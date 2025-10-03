#include "restselectworker.h"
#include "restclient.h"
#include "restclass.h"
#include "restreply.h"

RestSelectWorker::RestSelectWorker(QObject *parent):
    QSWorker(parent)
{

}

bool RestSelectWorker::doRun()
{
    if(!m_reply.isNull()) {
        m_reply->abort();
        m_reply->deleteLater();
    }

    if(m_class)
        m_class->deleteLater();

    RestClient* client = RestHelper::apiClient(m_connection);
    if(!client || !client->getAvailable())
    {
        QMetaObject::invokeMethod(this, [this](){ emit this->failed(404, "Network error", QString("%1 (%2)").arg("Client unavailable").arg(404)); }, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, [this](){ emit this->finished(404); }, Qt::QueuedConnection);
        return true;
    }

    m_class = new RestClass(m_path, client, this);
    if(!m_method.isEmpty() && !m_body.isEmpty())
        m_reply = m_class->call(RestHelper::PutVerb, m_method, QVariant(m_body), m_parameters, m_headers);
    else
        m_reply = m_class->call(RestHelper::GetVerb, m_method, {}, m_parameters, m_headers);
    m_reply->setParent(this);
    m_reply->setAutoDelete(true);
    m_reply->setAutoParse(m_autoParse);

    m_reply->onDownloadProgress(this, [this](qint64 bytesReceived, qint64 bytesTotal) {
        int progress = ((double)bytesReceived/double(bytesTotal))*10000;
        emit this->progress("Download", progress);
    });
    m_reply->onParsingProgress(this, [this](qint64 bytesReceived, qint64 bytesTotal) {
        int progress = ((double)bytesReceived/double(bytesTotal))*10000;
        emit this->progress("Analyse de la réponse", progress);
    });
    m_reply->onFailed(this, [this](int httpStatus, const QVariant &reply) {
        emit this->failed(httpStatus, "Request Failed! See JSON for more details!", reply);
    });
    m_reply->onError(this, [this](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant &reply) {
        QString networkError;
        switch (type) {
        case RestReplyErrorTypes::NoError:
            break;
        case RestReplyErrorTypes::Network:
            networkError = "Network error";
            break;
        case RestReplyErrorTypes::Parser:
            networkError = "JSON/CBOR parse error";
            break;
        case RestReplyErrorTypes::Failure:
            networkError = "Request failure";
            break;
        case RestReplyErrorTypes::Deserialization:
            networkError = "Deserialization error";
            break;
        default:
            networkError = "Unknown error";
            break;
        }
        const QString networkMessage = QString("%1 (%2)").arg(errorString).arg(code);
        emit this->failed(code, networkError, networkMessage);
    });
    m_reply->onSucceeded(this, [this](int status, const QVariant& reply) {
        emit this->succeeded(status, reply);
    });
    m_reply->onCanceled(this, [this]() {
        emit this->canceled();
    });
    m_reply->onFinished(this, [this](int status) {
        emit this->finished(status);
    }, Qt::QueuedConnection);

    return true;
}

bool RestSelectWorker::abort()
{
    if(!m_running)
        return true;

    if(m_reply.isNull())
        return true;

    m_reply->abort();

    return waitForFinished();
}

bool RestSelectWorker::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if(!m_running)
        return true;

    if(m_reply.isNull())
        return true;

    m_reply->waitForFinished(timeout, flags);

    return true;
}
