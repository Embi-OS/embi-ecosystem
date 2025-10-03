#include "happyserver.h"
#include "happyrouter.h"
#include "happycrudrouter.h"
#include "happy_log.h"

#include <QHttpServerResponder>
#include <QTcpServer>

HappyServer::HappyServer(QObject *parent) :
    QObject(parent),
    m_sqlSchemePreparator(new SqlSchemePreparator(this)),
    m_routers(this)
{
    m_routers.onInserted([this](int index, HappyRouter* router) {
        router->setRegistered(true);

        if(HappyCrudRouter* crudRouter = qobject_cast<HappyCrudRouter*>(router))
            m_sqlSchemePreparator->tables().append(crudRouter->sqlTablePreparator());
    });

    m_routers.onRemoved([this](int index, HappyRouter* router) {
        router->setRegistered(false);

        if(HappyCrudRouter* crudRouter = qobject_cast<HappyCrudRouter*>(router))
            m_sqlSchemePreparator->tables().remove(crudRouter->sqlTablePreparator());
    });
}

HappyServer::~HappyServer()
{
    QMultiHashIterator<QString, QWebSocket*> i(m_sockets);
    while(i.hasNext()) {
        i.next();
        QWebSocket* socket = i.value();
        socket->deleteLater();
    }
}

QHttpServerResponse HappyServer::missingHandler(const QHttpServerRequest& request)
{
    const QString method = QHttpServerRequest::staticMetaObject.enumerator(0).valueToKey((int)request.method());
    const QString path = request.url().path();
    const QString data = QString("The requested resource \"%1 %2\" was not found on this server").arg(method, path);

    return QHttpServerResponse(data, QHttpServerResponder::StatusCode::NotFound);
}

bool HappyServer::init()
{
    bool result = true;

    m_httpServer = new QHttpServer(this);

    m_httpServer->setMissingHandler(m_httpServer, [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
        responder.sendResponse(missingHandler(request));
    });
    m_httpServer->addAfterRequestHandler(m_httpServer, [] (const QHttpServerRequest &request, QHttpServerResponse &responder) {
        auto headers = responder.headers();
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowOrigin, "*");
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowMethods, "*");
        headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowHeaders, "*");
        // headers.append(QHttpHeaders::WellKnownHeader::CrossOriginOpenerPolicy, "same-origin");
        // headers.append(QHttpHeaders::WellKnownHeader::CrossOriginEmbedderPolicy, "require-corp");
        responder.setHeaders(std::move(headers));
    });

    if(!m_sqlSchemePreparator->run(m_sqlConnection, false))
    {
        HAPPYLOG_CRITICAL()<<"Failed to init database connection:"<<m_sqlConnection;
        return false;
    }

    for(HappyRouter* router: m_routers)
    {
        if(HappyCrudRouter* crudRouter = qobject_cast<HappyCrudRouter*>(router))
            m_crudsRouterMap.insert(crudRouter->getTableName(), crudRouter);
    }

    for(HappyRouter* router: m_routers)
    {
        result &= registerRouter(router);
    }

    return result;
}

bool HappyServer::start()
{
    HAPPYLOG_INFO()<<qLogLineMessage(m_name+"::start",'*');

    for(HappyRouter* router: m_routers)
    {
        router->setReady(true);
    }

    QTcpServer* tcpserver = new QTcpServer(this);
    if (!tcpserver->listen(m_external ? QHostAddress::Any : QHostAddress::Any, m_port) || !m_httpServer->bind(tcpserver)) {
        tcpserver->deleteLater();
        HAPPYLOG_CRITICAL()<<qPrintable(QString("%1 failed to listen on port %2").arg(m_name).arg(m_port));
        setReady(false);
        emit this->started(false);
        return false;
    }
    const auto port = tcpserver->serverPort();

    connect(m_httpServer, &QHttpServer::newWebSocketConnection, this, &HappyServer::onNewSocketConnection, Qt::UniqueConnection);

    HAPPYLOG_INFO()<<qPrintable(QString("%1: running on http://localhost:%2").arg(m_name).arg(port));

    HAPPYLOG_INFO()<<qLogLine('*');

    setReady(true);

    emit this->started(true);

    return true;
}

bool HappyServer::registerRouter(HappyRouter* router)
{
    if(router==nullptr)
        return false;

    HAPPYLOG_DEBUG()<<qPrintable(QString("%1: Registering HappyRouter ->").arg(m_name))<<router->getPath();

    router->setParent(this);
    QQmlEngine::setObjectOwnership(router, QQmlEngine::CppOwnership);

    if (router->getReady())
    {
        HAPPYLOG_WARNING()<<qPrintable(QString("%1:").arg(m_name))<<router->getPath()<<"already initialized!";
        return true;
    }

    if (!router->init(this))
    {
        HAPPYLOG_CRITICAL()<<"Failed to init model:"<<router->getPath();
        return false;
    }

    HAPPYLOG_INFO()<<qPrintable(QString("%1: HappyRouter ->").arg(m_name))<<router->getPath()<<"initialized";

    return true;
}

HappyCrudRouter* HappyServer::crudRouter(const QString& tableName) const
{
    return m_crudsRouterMap.value(tableName, nullptr);
}

QList<QWebSocket*> HappyServer::sockets(const QString& path) const
{
    return m_sockets.values(path);
}

void HappyServer::onNewSocketConnection()
{
    if(!m_httpServer->hasPendingWebSocketConnections())
        return;

    QWebSocket* socket = m_httpServer->nextPendingWebSocketConnection().release();

    HAPPYLOG_TRACE()<<"New web socket connection:"<<socket->requestUrl().path();

    connect(socket, &QWebSocket::textMessageReceived, this, &HappyServer::onSocketTextMessageReceived);
    connect(socket, &QWebSocket::binaryMessageReceived, this, &HappyServer::onSocketBinaryMessageReceived);
    connect(socket, &QWebSocket::disconnected, this, &HappyServer::onSocketDisconnected);

    const QString path = socket->requestUrl().path();
    m_sockets.insert(path, socket);

    emit this->socketConnected(path, socket);
}

void HappyServer::onSocketTextMessageReceived(const QString& message)
{
    QWebSocket *socket = qobject_cast<QWebSocket*>(sender());

    if(!socket)
        return;

    HAPPYLOG_TRACE()<<"Message received:"<<message;
}

void HappyServer::onSocketBinaryMessageReceived(const QByteArray& message)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    if(!socket)
        return;

    HAPPYLOG_TRACE()<<"Binary Message received:"<<message;
}

void HappyServer::onSocketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    if(!socket)
        return;

    HAPPYLOG_TRACE()<<"Web socket disconnected:"<<socket->requestUrl().path();

    const QString path = socket->requestUrl().path();

    emit this->socketDisconnected(path, socket);

    m_sockets.remove(path, socket);
    socket->deleteLater();
}
