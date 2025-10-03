#include "sqlmapper.h"
#include "sql_log.h"

#include "sqlselectworker.h"
#include "sqlsubmitworker.h"

SqlMapper::SqlMapper(QObject *parent):
    SqlMapper("", parent, &SqlMapper::staticMetaObject)
{

}

SqlMapper::SqlMapper(const QString& baseName, QObject *parent, const QMetaObject* blacklistedMetaObject) :
    QVariantMapper(baseName, parent, blacklistedMetaObject)
{
    connect(this, &SqlMapper::connectionChanged, this, &QVariantMapper::queueSelect);

    connect(this, &SqlMapper::methodFieldChanged, this, &QVariantMapper::queueSelect);
    connect(this, &SqlMapper::methodValueChanged, this, &QVariantMapper::queueSelect);

    connect(this, &SqlMapper::fieldsChanged, this, &QVariantMapper::queueSelect);
    connect(this, &SqlMapper::expandChanged, this, &QVariantMapper::queueSelect);
    connect(this, &SqlMapper::omitChanged, this, &QVariantMapper::queueSelect);
    connect(this, &SqlMapper::expandChanged, this, &QVariantMapper::queueSelect);
    connect(this, &SqlMapper::joinsChanged, this, &QVariantMapper::queueSelect);
}

SqlMapper::~SqlMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        if(!m_submitWorker.isNull()) {
            m_submitWorker->disconnectAll(this);
            m_submitWorker.clear();
        }
    }
}

QSWorker* SqlMapper::selectWorker() const
{
    return m_selectWorker.data();
}

QSWorker* SqlMapper::submitWorker() const
{
    return m_submitWorker.data();
}

bool SqlMapper::doSelect()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->disconnectAll(this);
        m_selectWorker.clear();
    }

    if(m_methodField.isEmpty() && m_methodValue.isNull()) {
        setStorage(QVariantMap());
        emitSelectDone(false);
        return false;
    }

    QSWorker* worker = createSelectWorker();

    return runSelectWorker(worker);
}

bool SqlMapper::doSubmit(const QStringList& dirtyKeys)
{
    Q_UNUSED(dirtyKeys)

    if(!m_submitWorker.isNull()) {
        m_submitWorker->disconnectAll(this);
        m_submitWorker.clear();
    }

    if(m_methodField.isEmpty() && m_methodValue.isNull()) {
        setStorage(getCache());
        emitSubmitDone(false);
        return false;
    }

    QSWorker* worker = createSubmitWorker();

    return runSubmitWorker(worker);
}

bool SqlMapper::runSelectWorker(QSWorker* worker)
{
    if(!worker)
    {
        SQLLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSelectWorker";
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
        const QVariantList list = reply.toList();
        setExists(!list.isEmpty());
        if(!m_exists) {
            SQLLOG_TRACE()<<this<<m_baseName<<"Select succeeded but reply is empty";
            return;
        }
        const QVariantMap storage = list.first().toMap();
        setStorage(storage);
        SQLLOG_TRACE()<<this<<m_baseName<<"Select succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(reply.toString());
        setExists(false);
        setStorage(QVariantMap());
        SQLLOG_WARNING()<<this<<m_baseName<<m_methodField<<m_methodValue<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        SQLLOG_DEBUG()<<"Select Canceled";
    });
    worker->onFinished(this, [this](int status) {
        resetDownload();
        resetProgress();
        emitSelectDone(status==QSqlError::NoError);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
}

bool SqlMapper::runSubmitWorker(QSWorker* worker)
{
    if(!worker)
    {
        SQLLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSubmitWorker";
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
        const QVariantList list = reply.toList();
        setExists(!list.isEmpty());
        if(list.isEmpty()) {
            SQLLOG_WARNING()<<this<<m_baseName<<"Submit succeeded but reply is empty";
            return;
        }
        const QVariantMap storage = list.first().toMap();
        setStorage(storage);
        SQLLOG_TRACE()<<this<<m_baseName<<"Submit succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(reply.toString());
        setStorage(getBackup());
        SQLLOG_WARNING()<<this<<m_baseName<<m_methodField<<m_methodValue<<"Submit failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this]() {
        resetCode();
        resetMessage();
        resetError();
        SQLLOG_DEBUG()<<"Submit Canceled";
    });
    worker->onFinished(this, [this](int status) {
        resetUpload();
        resetProgress();
        emitSubmitDone(status==QSqlError::NoError);
    });

    m_submitWorker = worker;

    return m_submitWorker->run();
}

QSWorker* SqlMapper::createSelectWorker()
{
    SqlSelectWorker* worker = new SqlSelectWorker(this);

    QVariantMap filters;
    filters.insert(m_methodField, m_methodValue);

    worker->setConnection(m_connection);
    worker->setTableName(m_baseName);
    worker->setFilters(filters);
    worker->setFields(m_fields);
    worker->setOmit(m_omit);
    worker->setExpand(m_expand);
    worker->setJoins(m_joins);
    worker->setLimit(1);
    worker->setAutoDelete(true);

    return worker;
}

QSWorker* SqlMapper::createSubmitWorker()
{
    SqlSubmitWorker* worker = new SqlSubmitWorker(this);

    worker->setConnection(m_connection);
    worker->setTableName(m_baseName);
    worker->setPrimaryField(m_primaryField);
    worker->setFields(m_fields);
    worker->setOmit(m_omit);
    worker->setExpand(m_expand);
    worker->setJoins(m_joins);
    worker->setSource(m_exists ? QVariantList({getCache()}) : QVariantList());
    worker->setDestination(QVariantList({getStorage()}));
    worker->setAutoDelete(true);

    return worker;
}
