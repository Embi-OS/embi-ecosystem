#include "restbrowser.h"

#include <QTimer>
#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QtConcurrentRun>
#endif

RestBrowser::RestBrowser(QObject *parent) :
    QObject(parent)
{
    connect(this, &RestBrowser::clientAboutToChange, this, &RestBrowser::onClientAboutToChange);
}

void RestBrowser::onClientAboutToChange(RestClient* oldClient, RestClient* newClient)
{
    if(oldClient)
        disconnect(oldClient, nullptr, this, nullptr);

    if(newClient)
    {
        connect(newClient->manager(), &QNetworkAccessManager::authenticationRequired, this, &RestBrowser::authenticateReply);
        connect(newClient->manager(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &RestBrowser::authenticateProxy);
    }
}

void RestBrowser::execute()
{
    if(!m_client)
    {
        qCritical()<<"No client has been given to the browser";
        return;
    }

    QElapsedTimer* timer=new QElapsedTimer;
    timer->start();

    const QVariant body = parseBody();
    RestReply *reply;
    if (body.isValid()) {
        reply = m_client->rootClass()->call(m_verb.toUtf8(),
                                            QUrl(m_path),
                                            body,
                                            m_params,
                                            m_headers);
    } else {
        reply = m_client->rootClass()->call(m_verb.toUtf8(),
                                            QUrl(m_path),
                                            {},
                                            m_params,
                                            m_headers);
    }
    reply->setParent(this);
    reply->setAutoDelete(true);
    reply->setAutoParse(false);

    resetNetworkCode();
    resetNetworkError();
    resetNetworkMessage();
    resetNetworkDebug();

    reply->onSucceeded(this, [this](int status, const QVariant &value){
        setNetworkCode(QString::number(status));
        processReply(value);
    });
    reply->onFailed(this, [this](int status, const QVariant &value){
        setNetworkCode(QString::number(status));
        setNetworkError("Request failure");
        setNetworkMessage("Request Failed! See JSON for more details!");
        processReply(value);
    });
    reply->onError(this, [this](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant &reply){
        switch (type) {
        case RestReplyErrorTypes::NoError:
            resetNetworkError();
            break;
        case RestReplyErrorTypes::Network:
            setNetworkError("Network error");
            break;
        case RestReplyErrorTypes::Parser:
            setNetworkError("JSON/CBOR parse error");
            break;
        case RestReplyErrorTypes::Failure:
            setNetworkError("Request failure");
            break;
        case RestReplyErrorTypes::Deserialization:
            setNetworkError("Deserialization error");
            break;
        default:
            setNetworkError("Unknown error");
            break;
        }
        setNetworkMessage(QString("%1 (%2)").arg(errorString).arg(code));
        processReply(reply);
    });
    reply->onFinished(this, [this, timer, reply](int status) {
        QTimer::singleShot(2000, this, &RestBrowser::zeroBars);
        setNetworkCode(QString::number(status));
        setNetworkDebug(QString("Query took: %1 [ms] for %2").arg(timer->nsecsElapsed()/1000000.0,2).arg(bytes(reply->getContentLength())));
        delete timer;
    });

    setReplyUrl(reply->networkReply()->url().toString());
    connect(reply, &RestReply::uploadProgress, this, [this](int c, int m){
        float progress=((float)c/(float)m)*10000;
        setUpload(BOUND(0., progress, 10000.));
    });
    connect(reply, &RestReply::downloadProgress, this, [this](int c, int m){
        float progress=((float)c/(float)m)*10000;
        setDownload(BOUND(0., progress, 10000.));
    });
    connect(reply, &RestReply::parsingProgress, this, [this](int c, int m){
        float progress=((float)c/(float)m)*10000;
        setParsing(BOUND(0., progress, 10000.));
    });
}

void RestBrowser::authenticateReply(QNetworkReply*, QAuthenticator *auth)
{

}

void RestBrowser::authenticateProxy(const QNetworkProxy&, QAuthenticator *auth)
{

}

void RestBrowser::processReply(const QVariant& value)
{
#ifdef QT_CONCURRENT_LIB

    QElapsedTimer* timer=new QElapsedTimer;
    timer->start();

    setReplyJson("Parsing reply...");
    const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(value.constData());
    auto future = QtConcurrent::run([](const QVariantMap& reply) {
        return RestReply::parseReply(reply);
    }, map);

    future.then(this, [this, timer](const QVariant& data) {
        setNetworkDebug(getNetworkDebug()+" - "+QString("Parsing took: %1 [ms]").arg(timer->nsecsElapsed()/1000000.0,2));
        delete timer;
        processViewText(data);
    });

#else

    QElapsedTimer timer;
    timer.start();

    const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(value.constData());
    const QVariant reply = RestReply::parseReply(map);

    processViewText(reply);

#endif
}

void RestBrowser::processViewText(const QVariant& value)
{
#ifdef QT_CONCURRENT_LIB

    QElapsedTimer* timer=new QElapsedTimer;
    timer->start();

    setReplyJson("Process reply to display...");
    auto future = QtConcurrent::run([](const QVariant& value) {
        return RestHelper::parseBody(value, false);
    }, value);
    future.then(this, [this, timer](const QString& text){
        setNetworkDebug(getNetworkDebug()+" - "+QString("Processing took: %1 [ms]").arg(timer->nsecsElapsed()/1000000.0,2));
        delete timer;
        setReplyJson(text);
        zeroBars();
    });

#else

    QElapsedTimer timer;
    timer.start();

    const QString text = RestHelper::parseBody(value, false);

    setReplyJson(text);
    zeroBars();

#endif
}

void RestBrowser::zeroBars()
{
    resetUpload();
    resetDownload();
    resetParsing();
}

QVariant RestBrowser::parseBody() const
{
    const QString data = m_bodyJson;
    if (data.isEmpty())
        return {};

    QJsonParseError error;
    const QVariant body = QUtils::Json::jsonToVariant(data.toUtf8(), &error);
    qWarning()<<error.errorString();

    return body;
}
