#include "happyfullrouter.h"
#include "happyserver.h"

HappyFullRouter::HappyFullRouter(QObject *parent) :
    HappyCrudRouter(parent)
{

}

QVariantMap HappyFullRouter::info()
{
    QVariantMap map = HappyCrudRouter::info();

    map.insert("socketPath", getSocketPath());

    return map;
}

bool HappyFullRouter::init(HappyServer* happyServer)
{
    bool result = HappyCrudRouter::init(happyServer);

    if(m_socketPath.isEmpty())
        return result;

    QHttpServer* server = happyServer->httpServer();
    if(!server)
        return false;

    server->addWebSocketUpgradeVerifier(server, [this](const QHttpServerRequest &request) {
        if (request.url().path().startsWith(QString("/%1/").arg(m_socketPath)))
            return QHttpServerWebSocketUpgradeResponse::accept();
        else
            return QHttpServerWebSocketUpgradeResponse::passToNext();
    });

    return result;
}

HappyReply HappyFullRouter::postObject(const QVariant& data, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::postObject(data, parameters, headers);

    sendSocket(reply, QVariant());

    return reply;
}

HappyReply HappyFullRouter::putObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::putObject(data, argValue, parameters, headers);

    sendSocket(reply, argValue);

    return reply;
}

HappyReply HappyFullRouter::patchObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::patchObject(data, argValue, parameters, headers);

    sendSocket(reply, argValue);

    return reply;
}

HappyReply HappyFullRouter::deleteObject(const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::deleteObject(argValue, parameters, headers);

    sendSocket(reply, argValue);

    return reply;
}

void HappyFullRouter::sendSocket(const HappyReply& reply, const QVariant& argValue)
{
    if(!reply.result || m_socketPath.isEmpty())
        return;

    const QVariantMap variant = reply.variant.toMap();
    const QVariant lookupValue = argValue.isNull() ? variant.value(m_lookupField) : argValue;

    QList<QWebSocket*> sockets;
    sockets.append(m_happyServer->sockets(QString("/%1/").arg(m_socketPath)));
    sockets.append(m_happyServer->sockets(QString("/%1/%2/").arg(m_socketPath, lookupValue.toString())));

    const QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QVariantMap event;
    event.insert("uuid", uuid);
    event.insert("timestamp", QDateTime::currentDateTime());
    event.insert("action", reply.action);
    event.insert("path", m_path);
    event.insert("ressource", lookupValue.toString());
    event.insert("data", variant);

    for(QWebSocket* socket: sockets) {
        const HappyHttpHeaders headers(socket->request());
        const QByteArray data = HappyReply(event).data(headers);

        QMetaObject::invokeMethod(socket, &QWebSocket::sendBinaryMessage, Qt::QueuedConnection, data);
    }
}
