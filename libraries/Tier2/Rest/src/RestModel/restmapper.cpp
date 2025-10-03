#include "restmapper.h"
#include "rest_log.h"

#include "restselectworker.h"
#include "restsubmitworker.h"

RestMapper::RestMapper(QObject *parent):
    RestMapper("", parent, &RestMapper::staticMetaObject)
{

}

RestMapper::RestMapper(const QString& baseName, QObject *parent, const QMetaObject* blacklistedMetaObject) :
    QVariantMapper(baseName, parent, blacklistedMetaObject)
{
    connect(this, &RestMapper::connectionChanged, this, &QVariantMapper::queueSelect);

    connect(this, &RestMapper::methodChanged, this, &QVariantMapper::queueSelect);
    // select must be called manually on bodyChanged
    // connect(this, &RestMapper::bodyChanged, this, &QVariantMapper::queueSelect);

    connect(this, &RestMapper::fieldsChanged, this, &QVariantMapper::queueSelect);
    connect(this, &RestMapper::expandChanged, this, &QVariantMapper::queueSelect);
    connect(this, &RestMapper::omitChanged, this, &QVariantMapper::queueSelect);
}

RestMapper::~RestMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        if(!m_submitWorker.isNull()) {
            m_submitWorker->disconnectAll(this);
            m_submitWorker.clear();
        }
    }
}

QSWorker* RestMapper::selectWorker() const
{
    return m_selectWorker.data();
}

QSWorker* RestMapper::submitWorker() const
{
    return m_submitWorker.data();
}

bool RestMapper::doSelect()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->disconnectAll(this);
        m_selectWorker.clear();
    }

    if(m_method.isEmpty()) {
        setStorage(QVariantMap());
        emitSelectDone(false);
        return false;
    }

    QSWorker* worker = createSelectWorker();

    return runSelectWorker(worker);
}

bool RestMapper::doSubmit(const QStringList& dirtyKeys)
{
    Q_UNUSED(dirtyKeys)

    if(!m_submitWorker.isNull()) {
        m_submitWorker->disconnectAll(this);
        m_submitWorker.clear();
    }

    if(m_method.isEmpty()) {
        setStorage(getCache());
        emitSubmitDone(false);
        return false;
    }

    QSWorker* worker = createSubmitWorker();
    bool ret = runSubmitWorker(worker);

    return ret;
}

bool RestMapper::runSelectWorker(QSWorker* worker)
{
    if(!worker)
    {
        RESTLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSelectWorker";
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
        setExists(true);
        const QVariantMap storage = reply.toMap();
        setStorage(storage);
        RESTLOG_TRACE()<<this<<m_baseName<<"Select succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        setExists(false);
        setStorage(QVariantMap());
        RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
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

bool RestMapper::runSubmitWorker(QSWorker* worker)
{
    if(!worker)
    {
        RESTLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSubmitWorker";
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
            RESTLOG_WARNING()<<this<<m_baseName<<"Submit succeeded but reply is empty";
            return;
        }
        const QVariantMap storage = list.first().toMap();
        setStorage(storage);
        RESTLOG_TRACE()<<this<<m_baseName<<"Submit succeeded";
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        setStorage(getBackup());
        RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Submit failed, Reason:"<<m_code<<m_error<<m_message;
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

QSWorker* RestMapper::createSelectWorker()
{
    RestSelectWorker* worker = new RestSelectWorker(this);

    QVariantMap parameters = m_parameters;

    if (!m_fields.isEmpty())
        parameters.insert("fields", m_fields.join(","));

    if (!m_expand.isEmpty())
        parameters.insert("expand", m_expand.join(","));

    if (!m_omit.isEmpty())
        parameters.insert("omit", m_omit.join(","));

    worker->setConnection(m_connection);
    worker->setPath(m_baseName);
    worker->setMethod(m_method);
    worker->setBody(m_body);
    worker->setParameters(parameters);
    worker->setHeaders(m_headers);
    worker->setAutoDelete(true);

    return worker;
}

QSWorker* RestMapper::createSubmitWorker()
{
    RestSubmitWorker* worker = new RestSubmitWorker(this);

    QVariantMap parameters = m_parameters;

    if (!m_fields.isEmpty())
        parameters.insert("fields", m_fields.join(","));

    if (!m_expand.isEmpty())
        parameters.insert("expand", m_expand.join(","));

    if (!m_omit.isEmpty())
        parameters.insert("omit", m_omit.join(","));

    worker->setConnection(m_connection);
    worker->setPath(m_baseName);
    worker->setMethod(m_method);
    worker->setParameters(parameters);
    worker->setHeaders(m_headers);
    worker->setSource(m_exists ? QVariantList({getCache()}) : QVariantList());
    worker->setDestination(QVariantList({getStorage()}));
    worker->setAutoDelete(true);

    return worker;
}
