#include "restclass.h"
#include "restclient.h"
#include "rest_log.h"

RestClass::RestClass(QObject *parent):
    RestClass("", parent)
{

}

RestClass::RestClass(const QString& path, QObject *parent):
    RestClass(path, RestHelper::defaultApiClient(), parent)
{

}

RestClass::RestClass(const QString& path, RestClient* client, QObject *parent):
    QObject(parent),
    m_client(client),
    m_path(path)
{

}

RestClass *RestClass::subClass(const QString &path, QObject *parent)
{
    RestClass* restClass = new RestClass(parent?parent:this);
    restClass->setClient(m_client);
    restClass->setPath(QUrl(m_path).resolved(path).toString());
    return restClass;
}

RestRequestBuilder RestClass::builder()
{
    return m_client->builder().addPath(m_path.split(QLatin1Char('/'), Qt::SkipEmptyParts));
}

RestReply* RestClass::call(QNetworkReply* networkReply, QObject* parent)
{
    resetDownloadProgress();
    resetUploadProgress();
    resetHttpStatus();
    resetErrorCode();
    resetErrorType();
    resetErrorString();
    resetNetworkError();
    resetReply();

    setLoading(true);

    QElapsedTimer* timer=new QElapsedTimer;
    timer->start();

    RestReply* reply = new RestReply(networkReply, parent);

    reply->setNetworkRequest(QString("%1: %2").arg(m_lastRequestBuilder.verb(),m_lastRequestBuilder.buildUrl().toString()));
    reply->setAutoDelete(m_replyAutoDelete);
    reply->setAllowEmpty(m_replyAllowEmpty);

    reply->onFinished([this, timer, reply](int){
        setLoading(false);
        RESTLOG_DEBUG()<<"REST request"<<reply->getNetworkRequest()<<"took"<<timer->nsecsElapsed()/1000000.0<<"ms for"<<bytes(reply->getContentLength());
        // qNotice()<<this<<"REST request"<<reply->getNetworkRequest()<<"took"<<timer->nsecsElapsed()/1000000.0<<"ms for"<<bytes(reply->getContentLength());
        delete timer;
    }, Qt::QueuedConnection);
    reply->onSucceeded([this](int httpStatus, const QVariant& reply){
        setHttpStatus(httpStatus);
        setReply(reply);
        setDownloadProgress(10000);
        setUploadProgress(10000);
    });
    reply->onFailed([this](int httpStatus, const QVariant& reply){
        setHttpStatus(httpStatus);
        setReply(reply);
        setDownloadProgress(10000);
        setUploadProgress(10000);
    });
    reply->onAllErrors([this](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant& reply){
        setErrorString(errorString);
        setErrorCode(code);
        setErrorType(type);
        setReply(reply);
    });
    reply->onSslErrors([](const QList<QSslError> &errors, bool ignoreErrors){
        // TODO:
    });
    reply->onDownloadProgress([this](qint64 bytesReceived, qint64 bytesTotal){
        const int progress = (double(bytesReceived)/double(bytesTotal))*10000;
        setDownloadProgress(progress);
    });
    reply->onUploadProgress([this](qint64 bytesSent, qint64 bytesTotal){
        const int progress = (double(bytesSent)/double(bytesTotal))*10000;
        setUploadProgress(progress);
    });

    return reply;
}

RestReply* RestClass::call(const QByteArray &verb, const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    return call(create(verb, methodPath, parameters, headers, paramsAsBody), nullptr);
}
RestReply* RestClass::call(const QByteArray &verb, const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(create(verb, methodPath, body, parameters, headers), nullptr);
}
RestReply* RestClass::call(const QByteArray &verb, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    return call(create(verb, parameters, headers, paramsAsBody), nullptr);
}
RestReply* RestClass::call(const QByteArray &verb, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(create(verb, body, parameters, headers), nullptr);
}
RestReply* RestClass::call(const QByteArray &verb, const QUrl &relativeUrl, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    return call(create(verb, relativeUrl, parameters, headers, paramsAsBody), nullptr);
}
RestReply* RestClass::call(const QByteArray &verb, const QUrl &relativeUrl, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(create(verb, relativeUrl, body, parameters, headers), nullptr);
}

RestReply* RestClass::get(const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::GetVerb, methodPath, {}, parameters, headers);
}
RestReply* RestClass::get(const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::GetVerb, "", {}, parameters, headers);
}

RestReply* RestClass::post(const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PostVerb, methodPath, body, parameters, headers);
}
RestReply* RestClass::post(const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PostVerb, "", body, parameters, headers);
}

RestReply* RestClass::put(const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PutVerb, methodPath, body, parameters, headers);
}
RestReply* RestClass::put(const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PutVerb, "", body, parameters, headers);
}

RestReply* RestClass::deleteResource(const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::DeleteVerb, methodPath, {}, parameters, headers);
}
RestReply* RestClass::deleteResource(const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::DeleteVerb, "", {}, parameters, headers);
}

RestReply* RestClass::patch(const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PatchVerb, methodPath, body, parameters, headers);
}
RestReply* RestClass::patch(const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::PatchVerb, "", body, parameters, headers);
}

RestReply* RestClass::head(const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::HeadVerb, methodPath, {}, parameters, headers);
}
RestReply* RestClass::head(const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::HeadVerb, "", {}, parameters, headers);
}

RestReply* RestClass::options(const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::OptionsVerb, methodPath, {}, parameters, headers);
}
RestReply* RestClass::options(const QVariantMap &parameters, const QVariantMap &headers)
{
    return call(RestHelper::OptionsVerb, "", {}, parameters, headers);
}

QNetworkReply* RestClass::create(RestRequestBuilder& builder)
{
    builder.setTimeout(m_replyTimeout);

    m_lastRequestBuilder = builder;

    return builder.send();
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    RestRequestBuilder cBuilder = builder()
        .addPath(methodPath)
        .addParameters(RestHelper::mapToQuery(parameters))
        .addHeaders(headers)
        .setBody(body)
        .setVerb(verb);
    return create(cBuilder);
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    RestRequestBuilder cBuilder = (paramsAsBody ?
         builder().addPostParameters(RestHelper::mapToQuery(parameters)) :
         builder().addParameters(RestHelper::mapToQuery(parameters)))
        .addPath(methodPath)
        .addHeaders(headers)
        .setVerb(verb);
    return create(cBuilder);
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    RestRequestBuilder cBuilder = (paramsAsBody ?
         builder().addPostParameters(RestHelper::mapToQuery(parameters)) :
         builder().addParameters(RestHelper::mapToQuery(parameters)))
        .addHeaders(headers)
        .setVerb(verb);
    return create(cBuilder);
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    RestRequestBuilder cBuilder = builder()
        .addParameters(RestHelper::mapToQuery(parameters))
        .addHeaders(headers)
        .setBody(body)
        .setVerb(verb);
    return create(cBuilder);
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QUrl &relativeUrl, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody)
{
    RestRequestBuilder cBuilder = (paramsAsBody ?
         builder().addPostParameters(RestHelper::mapToQuery(parameters)) :
         builder().addParameters(RestHelper::mapToQuery(parameters)))
        .updateFromRelativeUrl(relativeUrl, true)
        .addHeaders(headers)
        .setVerb(verb);
    return create(cBuilder);
}

QNetworkReply* RestClass::create(const QByteArray &verb, const QUrl &relativeUrl, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers)
{
    RestRequestBuilder cBuilder = builder()
        .updateFromRelativeUrl(relativeUrl, true)
        .addParameters(RestHelper::mapToQuery(parameters))
        .addHeaders(headers)
        .setBody(body)
        .setVerb(verb);
    return create(cBuilder);
}
