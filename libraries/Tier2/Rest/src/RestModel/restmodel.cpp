#include "restmodel.h"
#include "rest_log.h"

#include "restselectworker.h"
#include "restsubmitworker.h"

RestModel::RestModel(QObject *parent) :
    RestModel("", parent)
{

}

RestModel::RestModel(const QString& name, QObject *parent) :
    QVariantListModel(name, parent)
{
    connect(this, &RestModel::connectionChanged, this, &QVariantListModel::queueSelect);

    connect(this, &RestModel::filterEnabledChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::filterRoleNameChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::filterValueChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::filterMapChanged, this, &QVariantListModel::queueSelect);

    connect(this, &RestModel::sortEnabledChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::sortRoleNameChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::sortOrderChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::sortMapChanged, this, &QVariantListModel::queueSelect);

    connect(this, &RestModel::limitChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::offsetChanged, this, &QVariantListModel::queueSelect);

    connect(this, &RestModel::fieldsChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::omitChanged, this, &QVariantListModel::queueSelect);
    connect(this, &RestModel::expandChanged, this, &QVariantListModel::queueSelect);

    connect(this, &RestModel::paginationAboutToChange, this, &RestModel::onPaginationAboutToChange);
    connect(this, &RestModel::paginationChanged, this, &RestModel::onPaginationChanged);
}

RestModel::~RestModel()
{
    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

void RestModel::resize()
{
    if(m_selecting || m_limit<=0)
        return;

    setSelecting(true);

    if(count() > m_limit)
        remove(m_limit, count()-m_limit);

    setSelecting(false);
}

void RestModel::onPaginationAboutToChange(QSPagination* oldPagination, QSPagination* newPagination)
{
    if(oldPagination)
    {
        disconnect(oldPagination, nullptr, this, nullptr);
        disconnect(this, nullptr, oldPagination, nullptr);
    }
}

void RestModel::onPaginationChanged(QSPagination* pagination)
{
    if(pagination)
    {
        connect(pagination, &QSPagination::invalidated, this, &QVariantListModel::queueSelect);
    }
}

bool RestModel::doSelect()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
    }

    QSWorker* worker = createSelectWorker();

    return runSelectWorker(worker);
}

bool RestModel::doSubmit()
{
    if(!m_submitWorker.isNull()) {
        m_submitWorker->abort();
        m_submitWorker->deleteLater();
    }

    QSWorker* worker = createSubmitWorker();

    return runSubmitWorker(worker);
}

bool RestModel::doCancel()
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

bool RestModel::runSelectWorker(QSWorker* worker)
{
    if(!worker)
    {
        RESTLOG_WARNING()<<this<<m_baseName<<"No worker given to runSelectWorker";
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
        if(reply.canConvert<QVariantMap>())
        {
            QVariantMap result = reply.toMap();
            QVariantList storage = result.value("data").toList();

            if(m_pagination)
            {
                m_pagination->setTotal(result.value("total").toInt());
                m_pagination->setCount(result.value("page_size").toInt());
                m_pagination->setPageCount(result.value("page_count").toInt());
                m_pagination->setPrevious(result.value("previous").toString());
                m_pagination->setNext(result.value("next").toString());
            }

            setStorage(std::move(storage));
        }
        else if(reply.canConvert<QVariantList>())
        {
            QVariantList storage = reply.toList();
            setStorage(std::move(storage));
        }
        else
        {
            setStorage(QVariantList());
        }
        RESTLOG_TRACE()<<this<<m_baseName<<"Select succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        setStorage(QVariantList());
        RESTLOG_WARNING()<<this<<m_baseName<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        RESTLOG_DEBUG()<<"Select Canceled";
    });
    worker->onFinished(this, [this](int status) {
        resetDownload();
        resetProgress();
        emitSelectDone(status>=200 && status<=299);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
}

bool RestModel::runSubmitWorker(QSWorker* worker)
{
    if(!worker)
    {
        RESTLOG_WARNING()<<this<<m_baseName<<"No worker given to runSubmitWorker";
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
        RESTLOG_TRACE()<<this<<m_baseName<<"Submit succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        patchStorage(getBackup());
        RESTLOG_WARNING()<<this<<m_baseName<<"Submit failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        RESTLOG_DEBUG()<<"Submit Canceled";
    });
    worker->onFinished(this, [this](int status) {
        resetUpload();
        resetProgress();
        emitSubmitDone(status>=200 && status<=299);
    });

    m_submitWorker = worker;

    return m_submitWorker->run();
}

QSWorker* RestModel::createSelectWorker()
{
    RestSelectWorker* worker = new RestSelectWorker(this);

    worker->setConnection(m_connection);
    worker->setPath(m_baseName);
    worker->setMethod("");
    worker->setParameters(selectParameters());
    worker->setHeaders(m_headers);
    worker->setAutoDelete(true);

    return worker;
}

QSWorker* RestModel::createSubmitWorker()
{
    RestSubmitWorker* worker = new RestSubmitWorker(this);

    worker->setConnection(m_connection);
    worker->setPath(m_baseName);
    worker->setMethod("");
    worker->setParameters(submitParameters());
    worker->setHeaders(m_headers);
    worker->setPrimaryField(m_primaryField);
    worker->setSource(getBackup());
    worker->setDestination(getStorage());
    worker->setAutoDelete(true);

    return worker;
}

QVariantMap RestModel::selectParameters() const
{
    QVariantMap parameters = m_parameters;
    if(m_filterEnabled)
    {
        if(!m_filterRoleName.isEmpty()) {
            const QString value = RestHelper::formatFilterValue(m_filterValue, m_filterInverted);
            parameters.insert(m_filterRoleName, value);
        }

        for(auto [key, value]: m_filterMap.asKeyValueRange()) {
            parameters.insert(key, RestHelper::formatFilterValue(value, false));
        }
    }

    if(m_sortEnabled)
    {
        QVariantMap sorters = m_sortMap;
        if(!m_sortRoleName.isEmpty())
            sorters.insert(m_sortRoleName, m_sortOrder);

        QStringList sortList;
        for(auto [key, value]: sorters.asKeyValueRange()) {
            const QString sort = QString("%1%2").arg(value.toInt()==Qt::AscendingOrder?'+':'-').arg(key);
            sortList.append(sort);
        }
        if (!sortList.isEmpty())
            parameters.insert("sort", sortList.join(","));
    }

    if (!m_fields.isEmpty())
        parameters.insert("fields", m_fields.join(","));

    if (!m_expand.isEmpty())
        parameters.insert("expand", m_expand.join(","));

    if (!m_omit.isEmpty())
        parameters.insert("omit", m_omit.join(","));

    if (m_limit>0)
        parameters.insert("limit", m_limit);

    if (m_offset>0)
        parameters.insert("offset", m_offset);

    if(m_pagination && m_pagination->getEnabled() && m_pagination->getPerPage()>0 && m_pagination->getPage()>0)
    {
        parameters.insert("per_page", m_pagination->getPerPage());
        parameters.insert("page", m_pagination->getPage());
    }

    return parameters;
}

QVariantMap RestModel::submitParameters() const
{
    QVariantMap parameters = m_parameters;
    if (!m_fields.isEmpty())
        parameters.insert("fields", m_fields.join(","));

    if (!m_expand.isEmpty())
        parameters.insert("expand", m_expand.join(","));

    if (!m_omit.isEmpty())
        parameters.insert("omit", m_omit.join(","));

    if (m_limit>0)
        parameters.insert("limit", m_limit);

    if (m_offset>0)
        parameters.insert("offset", m_offset);

    return parameters;
}
