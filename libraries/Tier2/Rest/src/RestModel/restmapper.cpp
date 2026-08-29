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
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
        m_selectWorker.clear();
    }

    if(!hasSelectTarget()) {
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
        m_submitWorker->abort();
        m_submitWorker->deleteLater();
        m_submitWorker.clear();
    }

    if(!hasSubmitTarget()) {
        setStorage(getCache());
        emitSubmitDone(false);
        return false;
    }

    QSWorker* worker = createSubmitWorker();
    bool ret = runSubmitWorker(worker);

    return ret;
}

bool RestMapper::hasSelectTarget() const
{
    return !m_method.isEmpty();
}

bool RestMapper::hasSubmitTarget() const
{
    return !m_method.isEmpty();
}

bool RestMapper::runSelectWorker(QSWorker* worker)
{
    m_selectReplyAccepted = false;

    if(!worker)
    {
        RESTLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSelectWorker";
        emitSelectDone(false);
        return false;
    }

    worker->onProgress(this, [this, worker](const QString message, int progress) {
        if(worker != m_selectWorker)
            return;

        setMessage(message);
        setDownload(progress);
        setProgress(progress);
    });
    worker->onSucceeded(this, [this, worker](int status, const QVariant& reply) {
        if(worker != m_selectWorker)
            return;

        setCode(status);
        resetError();
        resetMessage();

        QString errorMessage;
        m_selectReplyAccepted = applySelectReply(reply, &errorMessage);
        if(!m_selectReplyAccepted) {
            setError(errorMessage);
            clearSelectData();
            RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Select reply parsing failed, Reason:"<<m_error;
            emit this->error();
        }
        RESTLOG_TRACE()<<this<<m_baseName<<"Select succeeded";
    });
    worker->onFailed(this, [this, worker](int status, const QString& error, const QVariant& reply) {
        if(worker != m_selectWorker)
            return;

        m_selectReplyAccepted = false;
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        clearSelectData();
        RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this, worker]() {
        if(worker != m_selectWorker)
            return;

        m_selectReplyAccepted = false;
        resetCode();
        resetMessage();
        resetError();
        RESTLOG_DEBUG()<<"Select Canceled";
    });
    worker->onFinished(this, [this, worker](int status) {
        if(worker != m_selectWorker)
            return;

        resetDownload();
        resetProgress();
        emitSelectDone(status>=200 && status<=299 && m_selectReplyAccepted);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
}

bool RestMapper::applySelectReply(const QVariant& reply, QString* errorMessage)
{
    if(reply.metaType().id() != QMetaType::QVariantMap) {
        if(errorMessage)
            *errorMessage = QStringLiteral("Invalid select reply: expected a map");
        return false;
    }

    setStorage(reply.toMap());
    setExists(true);
    return true;
}

void RestMapper::clearSelectData()
{
    setExists(false);
    setStorage(QVariantMap());
}

bool RestMapper::runSubmitWorker(QSWorker* worker)
{
    m_submitReplyAccepted = false;

    if(!worker)
    {
        RESTLOG_CRITICAL()<<this<<m_baseName<<"No worker given to runSubmitWorker";
        emitSubmitDone(false);
        return false;
    }

    worker->onProgress(this, [this, worker](const QString message, int progress) {
        if(worker != m_submitWorker)
            return;

        setMessage(message);
        setUpload(progress);
        setProgress(progress);
    });
    worker->onSucceeded(this, [this, worker](int status, const QVariant& reply) {
        if(worker != m_submitWorker)
            return;

        setCode(status);
        resetError();
        resetMessage();

        QString errorMessage;
        m_submitReplyAccepted = applySubmitReply(status, reply, &errorMessage);
        if(!m_submitReplyAccepted) {
            setError(errorMessage);
            restoreSubmitData();
            RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Submit reply parsing failed, Reason:"<<m_error;
            emit this->error();
        }
        RESTLOG_TRACE()<<this<<m_baseName<<"Submit succeeded";
    });
    worker->onFailed(this, [this, worker](int status, const QString& error, const QVariant& reply) {
        if(worker != m_submitWorker)
            return;

        m_submitReplyAccepted = false;
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        restoreSubmitData();
        RESTLOG_WARNING()<<this<<m_baseName<<m_method<<"Submit failed, Reason:"<<m_code<<m_error<<m_message;
        emit this->error();
    });
    worker->onCanceled(this, [this, worker]() {
        if(worker != m_submitWorker)
            return;

        m_submitReplyAccepted = false;
        resetCode();
        resetMessage();
        resetError();
        RESTLOG_DEBUG()<<"Submit Canceled";
    });
    worker->onFinished(this, [this, worker](int status) {
        if(worker != m_submitWorker)
            return;

        resetUpload();
        resetProgress();
        emitSubmitDone(status>=200 && status<=299 && m_submitReplyAccepted);
    });

    m_submitWorker = worker;

    return m_submitWorker->run();
}

bool RestMapper::applySubmitReply(int status, const QVariant& reply, QString* errorMessage)
{
    if(status == 204 && !reply.isValid()) {
        setExists(false);
        return true;
    }

    if(reply.metaType().id() != QMetaType::QVariantList) {
        if(errorMessage)
            *errorMessage = QStringLiteral("Invalid submit reply: expected a list");
        return false;
    }

    const auto* list = static_cast<const QVariantList*>(reply.constData());
    if(list->isEmpty()) {
        setExists(false);
        RESTLOG_WARNING()<<this<<m_baseName<<"Submit succeeded but reply is empty";
        return true;
    }

    const QVariant& first = list->first();
    if(first.metaType().id() != QMetaType::QVariantMap) {
        if(errorMessage)
            *errorMessage = QStringLiteral("Invalid submit reply: expected a map in the list");
        return false;
    }

    setStorage(first.toMap());
    setExists(true);
    return true;
}

void RestMapper::restoreSubmitData()
{
    setStorage(getBackup());
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
