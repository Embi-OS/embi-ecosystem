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
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
    }

    setLoading(true);

    QSWorker* worker = createSelectWorker();

    runSelectWorker(worker);
}

void RestArrayModel::cancel()
{
    if(!m_selectWorker.isNull()) {
        m_selectWorker->abort();
        m_selectWorker->deleteLater();
    }
}

bool RestArrayModel::runSelectWorker(QSWorker* worker)
{
    if(!worker)
    {
        RESTLOG_WARNING()<<this<<m_baseName<<"No worker given to runSelectWorker";
        emit this->invalidated(false);
        setLoading(false);
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

        RestParserError restParseError;
        const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(reply.constData());
        const QByteArray contentType = RestReply::parseContentType(map.value("contentType").toByteArray(), &restParseError);
        const QByteArray content = map.value("content").toByteArray();
        if (contentType == RestHelper::ContentTypeCbor)
        {
            setCbor(content);
        }
        else if (contentType == RestHelper::ContentTypeJson)
        {
            setJson(content);
        }
        else
        {
            setError(QString("Unsupported content type: %1").arg(QString::fromUtf8(contentType)));
            clear();
            RESTLOG_WARNING()<<this<<m_baseName<<"Select failed, Reason:"<<m_code<<m_error;
            emit this->error();
        }
    });
    worker->onFailed(this, [this](int status, const QString& error, const QVariant& reply) {
        setCode(status);
        setError(error);
        setMessage(RestHelper::parseBody(reply));
        clear();
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
        emit this->invalidated(status>=200 && status<=299);
        setLoading(false);
    });

    m_selectWorker = worker;

    return m_selectWorker->run();
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
