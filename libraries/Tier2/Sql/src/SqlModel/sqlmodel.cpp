#include "sqlmodel.h"
#include "sql_log.h"

#include "sqlselectworker.h"
#include "sqlsubmitworker.h"

SqlModel::SqlModel(QObject *parent) :
    SqlModel("", parent)
{

}

SqlModel::SqlModel(const QString& name, QObject *parent) :
    QVariantListModel(name, parent)
{
    connect(this, &SqlModel::connectionChanged, this, &QVariantListModel::queueSelect);

    connect(this, &SqlModel::filterEnabledChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::filterRoleNameChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::filterValueChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::filterMapChanged, this, &QVariantListModel::queueSelect);

    connect(this, &SqlModel::sortEnabledChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::sortRoleNameChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::sortOrderChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::sortMapChanged, this, &QVariantListModel::queueSelect);

    connect(this, &SqlModel::limitChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::offsetChanged, this, &QVariantListModel::queueSelect);

    connect(this, &SqlModel::fieldsChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::omitChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::expandChanged, this, &QVariantListModel::queueSelect);
    connect(this, &SqlModel::joinsChanged, this, &QVariantListModel::queueSelect);

    connect(this, &SqlModel::paginationAboutToChange, this, &SqlModel::onPaginationAboutToChange);
    connect(this, &SqlModel::paginationChanged, this, &SqlModel::onPaginationChanged);
}

SqlModel::~SqlModel()
{
    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

void SqlModel::resize()
{
    if(m_selecting || m_limit<=0)
        return;

    setSelecting(true);

    if(count() > m_limit)
        remove(m_limit, count()-m_limit);

    setSelecting(false);
}

void SqlModel::onPaginationAboutToChange(QSPagination* oldPagination, QSPagination* newPagination)
{
    if(oldPagination)
    {
        disconnect(oldPagination, nullptr, this, nullptr);
        disconnect(this, nullptr, oldPagination, nullptr);
    }
}

void SqlModel::onPaginationChanged(QSPagination* pagination)
{
    if(pagination)
    {
        connect(pagination, &QSPagination::invalidated, this, &QVariantListModel::queueSelect);
    }
}

bool SqlModel::doSelect()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
    }

    QSWorker* worker = createSelectWorker();

    return runSelectWorker(worker);
}

bool SqlModel::doSubmit()
{
    if(!m_submitWorker.isNull()) {
        m_submitWorker->abort();
        m_submitWorker->deleteLater();
    }

    QSWorker* worker = createSubmitWorker();

    return runSubmitWorker(worker);
}

bool SqlModel::doCancel()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
    }

    if(!m_submitWorker.isNull()) {
        m_submitWorker->abort();
        m_submitWorker->deleteLater();
    }

    return true;
}

bool SqlModel::runSelectWorker(QSWorker* worker)
{
    if(!worker)
    {
        SQLLOG_WARNING()<<this<<m_baseName<<"No worker given to runSelectWorker";
        emitSelectDone(false);
        return false;
    }

    worker->onProgress(this, [this](const QString message, int progress) {
        setMessage(message);
        setDownload(progress);
        setProgress(progress);
    });
    worker->onSucceeded(this, [this](int status, const QVariant& reply) {
        setCode(status);
        resetError();
        resetMessage();
        if(m_pagination && m_pagination->getEnabled())
        {
            QVariantMap result = reply.toMap();
            QVariantList storage = result.value("data").toList();

            int count = storage.size();
            int total = result.value("count").toInt();
            int pageCount = result.value("page_count").toInt();
            QString previous = result.value("previous").toString();
            QString next = result.value("next").toString();

            m_pagination->setCount(count);
            m_pagination->setTotal(total);
            m_pagination->setPageCount(pageCount);
            m_pagination->setPrevious(previous);
            m_pagination->setNext(next);
            setStorage(std::move(storage));
        }
        else
        {
            QVariantList storage = reply.toList();
            setStorage(std::move(storage));
        }
        SQLLOG_TRACE()<<this<<m_baseName<<"Select succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(reply.toString());
        setStorage(QVariantList());
        SQLLOG_WARNING()<<this<<m_baseName<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        SQLLOG_DEBUG()<<"Select Canceled";
    });
    worker->onFinished(this, [this](int) {
        resetDownload();
        resetProgress();
        emitSelectDone(true);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
}

bool SqlModel::runSubmitWorker(QSWorker* worker)
{
    if(!worker)
    {
        SQLLOG_WARNING()<<this<<m_baseName<<"No worker given to runSubmitWorker";
        emitSubmitDone(false);
        return false;
    }

    worker->onProgress(this, [this](const QString message, int progress) {
        setMessage(message);
        setUpload(progress);
        setProgress(progress);
    });
    worker->onSucceeded(this, [this](int status, const QVariant& reply) {
        setCode(status);
        resetError();
        resetMessage();
        QVariantList storage = reply.toList();
        patchStorage(std::move(storage));
        SQLLOG_TRACE()<<this<<m_baseName<<"Submit succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(reply.toString());
        patchStorage(getBackup());
        SQLLOG_WARNING()<<this<<m_baseName<<"Submit failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        SQLLOG_DEBUG()<<"Submit Canceled";
    });
    worker->onFinished(this, [this](int) {
        resetUpload();
        resetProgress();
        emitSubmitDone(true);
    });

    m_submitWorker = worker;

    return m_submitWorker->run();
}

QSWorker* SqlModel::createSelectWorker()
{
    SqlSelectWorker* worker = new SqlSelectWorker(this);

    QVariantMap filters;
    if(m_filterEnabled)
    {
        filters = m_filterMap;
        if(!m_filterRoleName.isEmpty())
            filters.insert(m_filterRoleName, m_filterValue);
    }

    QVariantMap sorters;
    if(m_sortEnabled)
    {
        sorters = m_sortMap;
        if(!m_sortRoleName.isEmpty())
            sorters.insert(m_sortRoleName, m_sortOrder);
    }

    worker->setConnection(m_connection);
    worker->setTableName(m_baseName);
    worker->setFilterInverted(m_filterInverted);
    worker->setFilters(filters);
    worker->setSorters(sorters);
    worker->setFields(m_fields);
    worker->setOmit(m_omit);
    worker->setExpand(m_expand);
    worker->setJoins(m_joins);
    worker->setLimit(m_limit);
    worker->setOffset(m_offset);
    if(m_pagination && m_pagination->getEnabled() && m_pagination->getPerPage()>0 && m_pagination->getPage()>0)
    {
        worker->setPerPage(m_pagination->getPerPage());
        worker->setPage(m_pagination->getPage());
    }
    worker->setAutoDelete(true);

    return worker;
}

QSWorker* SqlModel::createSubmitWorker()
{
    SqlSubmitWorker* worker = new SqlSubmitWorker(this);

    worker->setConnection(m_connection);
    worker->setTableName(m_baseName);
    worker->setPrimaryField(m_primaryField);
    worker->setFields(m_fields);
    worker->setOmit(m_omit);
    worker->setExpand(m_expand);
    worker->setJoins(m_joins);
    worker->setSource(getBackup());
    worker->setDestination(getStorage());
    worker->setAutoDelete(true);

    return worker;
}
