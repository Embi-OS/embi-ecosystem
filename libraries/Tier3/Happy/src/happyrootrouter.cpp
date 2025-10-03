#include "happyrootrouter.h"
#include "happyserver.h"

HappyRootRouter::HappyRootRouter(QObject *parent) :
    HappyRouter(parent)
{
    m_path = "/";
}

bool HappyRootRouter::init(HappyServer* happyServer)
{
    bool result = HappyRouter::init(happyServer);

    if(!m_happyServer)
        return false;

    QHttpServer* server = m_happyServer->httpServer();
    if(!server)
        return false;

    server->route("/", QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request) {
                      HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return getRootRoute(happyRequest);
                  });

    server->route(QString("/%1/").arg(m_path), QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request) {
                      HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return getBaseRoute(happyRequest);
                  });

    if(m_socketPath.isEmpty())
        return result;

    server->addWebSocketUpgradeVerifier(server, [this](const QHttpServerRequest &request) {
        if (request.url().path()==QString("/%1/").arg(m_socketPath))
            return QHttpServerWebSocketUpgradeResponse::accept();
        else
            return QHttpServerWebSocketUpgradeResponse::passToNext();
    });

    return result;
}

HappyReply HappyRootRouter::getRoot()
{
    QVariantMap reply;
    QVariantList models;

    for(HappyRouter* model : m_happyServer->routers())
    {
        models.append(model->getPath());
    }

    reply.insert("__HappyServer__", m_happyServer->getName());
    reply.insert("URLs", models);

    return HappyReply(reply);
}

HappyReply HappyRootRouter::getBase()
{
    QVariantMap reply;
    QVariantList models;

    for(HappyRouter* model : m_happyServer->routers())
    {
        models.append(model->info());
    }

    reply.insert("__HappyServer__", m_happyServer->getName());
    reply.insert("models", models);

    return HappyReply(reply);
}

QHttpServerResponse HappyRootRouter::getRootRoute(const HappyHttpRequest &request)
{
    const HappyReply reply = getRoot();

    return reply.response(request.headers);
}

QHttpServerResponse HappyRootRouter::getBaseRoute(const HappyHttpRequest &request)
{
    const HappyReply reply = getBase();

    return reply.response(request.headers);
}
