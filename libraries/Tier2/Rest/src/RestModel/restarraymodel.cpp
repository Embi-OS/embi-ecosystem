#include "restarraymodel.h"
#include "rest_log.h"

#include "restselectworker.h"
#include "restreply.h"

RestArrayModel::RestArrayModel(QObject *parent) :
    RestArrayModel("", parent)
{

}

RestArrayModel::RestArrayModel(const QString& name, QObject *parent) :
    QVariantReaderModel(parent),
    m_baseName(name)
{
    connect(this, &RestArrayModel::enabledChanged, this, &RestArrayModel::queueInvalidate);

    connect(this, &RestArrayModel::connectionChanged, this, &RestArrayModel::queueInvalidate);

    connect(this, &RestArrayModel::filterEnabledChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::filterRoleNameChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::filterValueChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::filterMapChanged, this, &RestArrayModel::queueInvalidate);

    connect(this, &RestArrayModel::sortEnabledChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::sortRoleNameChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::sortOrderChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::sortMapChanged, this, &RestArrayModel::queueInvalidate);

    connect(this, &RestArrayModel::limitChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::offsetChanged, this, &RestArrayModel::queueInvalidate);

    connect(this, &RestArrayModel::fieldsChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::omitChanged, this, &RestArrayModel::queueInvalidate);
    connect(this, &RestArrayModel::expandChanged, this, &RestArrayModel::queueInvalidate);
}

void RestArrayModel::queueInvalidate()
{
    if(!isCompleted() || !m_enabled)
        return;

    if(m_manual)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &RestArrayModel::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void RestArrayModel::invalidate()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->disconnectAll(this);
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
        m_selectWorker.clear();
    }

    setLoading(true);

    QSWorker* worker = createSelectWorker();

    runSelectWorker(worker);
}

void RestArrayModel::cancel()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->disconnectAll(this);
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
        m_selectWorker.clear();
    }

    m_selectReplyAccepted = false;
    setLoading(false);
}

bool RestArrayModel::runSelectWorker(QSWorker* worker)
{
    m_selectReplyAccepted = false;

    if(!worker)
    {
        RESTLOG_WARNING()<<this<<m_baseName<<"No worker given to runSelectWorker";
        emit this->invalidated(false);
        setLoading(false);
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
            RESTLOG_WARNING()<<this<<m_baseName<<"Select failed, Reason:"<<m_code<<m_error;
            emit this->error();
        }
    });
    worker->onFailed(this, [this, worker](int status, const QString& error, const QVariant& reply) {
        if(worker != m_selectWorker)
            return;

        m_selectReplyAccepted = false;
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        clearSelectData();
        RESTLOG_WARNING()<<this<<m_baseName<<"Select failed, Reason:"<<m_code<<m_error<<m_message;
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
        emit this->invalidated(status>=200 && status<=299 && m_selectReplyAccepted);
        setLoading(false);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
}

bool RestArrayModel::applySelectReply(const QVariant& reply, QString* errorMessage)
{
    if(reply.metaType().id() != QMetaType::QVariantMap) {
        if(errorMessage)
            *errorMessage = QStringLiteral("Invalid select reply: expected a map");
        return false;
    }

    const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(reply.constData());
    RestParserError restParseError;
    const QByteArray contentType = RestReply::parseContentType(map.value("contentType").toByteArray(), &restParseError);
    if(restParseError.code != 0) {
        if(errorMessage)
            *errorMessage = restParseError.error;
        return false;
    }

    const QByteArray content = map.value("content").toByteArray();
    if(contentType == RestHelper::ContentTypeCbor) {
        if(setCbor(content))
            return true;
    } else if(contentType == RestHelper::ContentTypeJson) {
        if(setJson(content))
            return true;
    } else {
        if(errorMessage)
            *errorMessage = QString("Unsupported content type: %1").arg(QString::fromUtf8(contentType));
        return false;
    }

    if(errorMessage)
        *errorMessage = QStringLiteral("Unable to parse select reply content");
    return false;
}

void RestArrayModel::clearSelectData()
{
    clear();
}

QSWorker* RestArrayModel::createSelectWorker()
{
    RestSelectWorker* worker = new RestSelectWorker(this);

    worker->setConnection(m_connection);
    worker->setPath(m_baseName);
    worker->setMethod("");
    worker->setParameters(selectParameters());
    worker->setHeaders(m_headers);
    worker->setAutoDelete(true);
    worker->setAutoParse(false);

    return worker;
}

QVariantMap RestArrayModel::selectParameters() const
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

    return parameters;
}
