#include "restsubmitworker.h"
#include "restclient.h"
#include "restclass.h"
#include "rest_log.h"

#include "syncable/qsdiffrunner.h"

RestSubmitWorker::RestSubmitWorker(QObject *parent):
    QSWorker(parent),
    m_class(new RestClass(this))
{

}

void RestSubmitWorker::sendNextRequest()
{
    if(!m_reply.isNull()) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply.clear();
    }

    while(m_reply.isNull() && m_patchIndex<m_patches.size())
    {
        const QSPatch patch = m_patches.at(m_patchIndex);
        m_reply = this->patch(patch);
        m_patchIndex++;
    }

    if(m_reply.isNull()) {
        emit this->succeeded(0, m_source);
        emit this->finished(0);
        return;
    }

    m_reply->setParent(this);
    m_reply->setAutoDelete(true);

    m_reply->onUploadProgress(this, [this](qint64 bytesReceived, qint64 bytesTotal) {
        double queryProgress = ((double)bytesReceived/double(bytesTotal))*100.0;
        double progress = ((double)(m_patchIndex-1)/(double)m_patches.size())*100.0 + queryProgress/m_patches.size();
        emit this->progress("Upload", progress*100);
    }, Qt::SingleShotConnection);
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
        int progress = ((double)(m_patchIndex)/(double)m_patches.size())*10000;
        emit this->progress("", progress);
    });
    m_reply->onCanceled(this, [this]() {
        emit this->canceled();
    });
    m_reply->onFinished(this, [this](int) {
        m_reply.clear();
        sendNextRequest();
    }, Qt::QueuedConnection);
}

RestReply* RestSubmitWorker::patch(const QSPatch& patch)
{
    RestReply* reply = nullptr;
    switch (patch.type()) {
    case QSPatch::Remove:
        reply = remove(patch.from(), patch.count());
        break;
    case QSPatch::Insert:
        reply = insert(patch.from(), patch.data());
        break;
    case QSPatch::Move:
        reply = move(patch.from(), patch.to(), patch.count());
        break;
    case QSPatch::Update: {
        if (patch.data().isEmpty())
            break;
        const QVariantMap diff = patch.data().at(0).toMap();
        reply = set(patch.from(), diff);
        break;
    }
    default:
        reply = nullptr;
        break;
    }
    return reply;
}

RestReply* RestSubmitWorker::insert(int index, const QVariant& variant)
{
    QVariantList variants;
    if(variant.metaType().id()!=QMetaType::QString && variant.canConvert<QVariantList>())
        variants = variant.toList();
    else
        variants.append(variant);

    if(variants.size()<=0)
        return nullptr;

    if(variants.size()>1)
    {
        RESTLOG_WARNING()<<"INSERT only the first item will be inserted";
    }

    QVariantMap map = variants.takeAt(0).toMap();
    RestReply* reply = m_class->call(RestHelper::PostVerb, "", QVariant(map), m_parameters, m_headers);

    reply->onSucceeded(this, [this, map, index, variants](int, const QVariant& reply) mutable {
        m_source.reserve(m_source.count() + variants.count());
        const QVariantMap data = reply.toMap();
        for(auto [key, value]: data.asKeyValueRange()) {
            map.insert(key, value);
        }
        m_source.insert(index, map);
        int offset = 1;
        for(const QVariant& variant: std::as_const(variants))
        {
            const int idx = index+offset;
            m_source.insert(idx, variant);
            offset++;
        }
    });

    return reply;
}

RestReply* RestSubmitWorker::set(int index, const QVariant& variant)
{
    QVariantMap original = m_source.at(index).toMap();
    const QString primaryValue = m_primaryField.isEmpty() ? m_method : original.value(m_primaryField).toString();
    const QVariantMap map = variant.toMap();

    RestReply* reply = m_class->call(RestHelper::PatchVerb, primaryValue, QVariant(map), m_parameters, m_headers);

    reply->onSucceeded(this, [this, original, index, variant](int, const QVariant& reply) mutable {
        const QVariantMap data = reply.toMap();
        for (auto [key, value] : data.asKeyValueRange()) {
            original.insert(key, value);
        }
        m_source.replace(index, original);
    });

    return reply;
}

RestReply* RestSubmitWorker::move(int from, int to, int count)
{
    for(int i = 0; i < count; ++i)
    {
        m_source.move(from, to);
    }

    RESTLOG_TRACE()<<"MOVE not supported";

    return nullptr;
}

RestReply* RestSubmitWorker::remove(int index, int count)
{
    if(count<=0)
        return nullptr;

    if(count>1)
    {
        RESTLOG_WARNING()<<"REMOVE only the first item will be removed";
    }

    const QVariantMap original = m_source.at(index).toMap();
    const QString primaryValue = m_primaryField.isEmpty() ? m_method : original.value(m_primaryField).toString();

    RestReply* reply = m_class->call(RestHelper::DeleteVerb, primaryValue, {}, m_parameters, m_headers);

    reply->onSucceeded(this, [this, index, count](int, const QVariant&) {
        m_source.remove(index, count);
    });

    return reply;
}

bool RestSubmitWorker::doRun()
{
    m_patches.clear();
    m_patchIndex = 0;

    if(!m_reply.isNull()) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply.clear();
    }

    if(m_class)
        m_class->deleteLater();

    const QSPatchSet patches = QSDiffRunner::compare(m_source, m_destination, m_primaryField);
    m_patches = QSDiffRunner::split(patches);

    if(m_patches.isEmpty())
    {
        emit this->finished(0);
        return true;
    }

    RestClient* client = RestHelper::apiClient(m_connection);
    if(!client || !client->getAvailable())
    {
        QMetaObject::invokeMethod(this, [this](){ emit this->failed(404, "Network error", QString("%1 (%2)").arg("Client unavailable").arg(404)); }, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, [this](){ emit this->finished(404); }, Qt::QueuedConnection);
        return true;
    }

    m_class = new RestClass(m_path, client, this);
    RestRequestBuilder builder = m_class->builder();
    builder.addHeaders(m_headers);
    builder.addParameters(m_parameters);

    sendNextRequest();

    return true;
}

bool RestSubmitWorker::abort()
{
    return waitForFinished();
}

bool RestSubmitWorker::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if(!m_running)
        return true;

    QEventLoop loop;
    connect(this, &QSWorker::finished, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);

    return loop.exec(flags);
}
