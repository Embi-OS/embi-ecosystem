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

    queueSendSocket(reply, QVariant());

    return reply;
}

HappyReply HappyFullRouter::putObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::putObject(data, argValue, parameters, headers);

    queueSendSocket(reply, argValue);

    return reply;
}

HappyReply HappyFullRouter::patchObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::patchObject(data, argValue, parameters, headers);

    queueSendSocket(reply, argValue);

    return reply;
}

HappyReply HappyFullRouter::deleteObject(const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    HappyReply reply = HappyCrudRouter::deleteObject(argValue, parameters, headers);

    queueSendSocket(reply, argValue);

    return reply;
}

void HappyFullRouter::queueSendSocket(const HappyReply& reply, const QVariant& argValue)
{
    QMetaObject::invokeMethod(this, &HappyFullRouter::sendSocket, Qt::QueuedConnection, reply, argValue);
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

    sendSockets(sockets, reply.action, lookupValue, variant);
}

void HappyFullRouter::sendSockets(const QList<QWebSocket*>& sockets, const QString& action, const QVariant& lookupValue, const QVariantMap object)
{
    const QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QVariantMap event;
    event.insert("uuid", uuid);
    event.insert("timestamp", QDateTime::currentDateTime());
    event.insert("action", action);
    event.insert("path", m_path);
    event.insert("ressource", lookupValue.toString());
    event.insert("data", object);

    for(QWebSocket* socket: sockets) {
        const QUrlQuery socketQuery = QUrlQuery(socket->requestUrl());
        const HappyHttpHeaders headers(socket->request());

        // Full method
        // const HappyHttpParameters parameters(socketQuery);
        // const QVariantMap data = getValues(lookupValue, parameters);
        // event.insert("data", data);

        // Optimized method
        // const HappyHttpParameters parameters(socketQuery);
        // const QStringList columns = parseColumns(parameters.fields, parameters.omit);
        // QVariantMap data;
        // for(const QString& column: columns) {
        //     if(!object.contains(column))
        //         continue;
        //     data.insert(column, object.value(column));
        // }
        // event.insert("data", data);

        const QByteArray message = HappyReply(event).data(headers);

        socket->sendBinaryMessage(message);
    }
}
