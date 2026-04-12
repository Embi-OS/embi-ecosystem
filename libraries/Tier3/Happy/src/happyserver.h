#ifndef HAPPYSERVER_H
#define HAPPYSERVER_H

#include <QDefs>
#include <QModels>
#include <Sql>
#include <Axion>

#include "happymigration.h"
#include "happyrouter.h"
#include "happycrudrouter.h"
#include "happyfullrouter.h"

class HappyServer : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, sqlConnection, SqlConnection, SqlDefaultConnection)

    Q_CONSTANT_PTR_PROPERTY(SqlSchemePreparator, sqlSchemePreparator)
    Q_CONSTANT_PTR_PROPERTY(SqlTablePreparator, sqlTablePreparator)
    Q_CONSTANT_OLP_PROPERTY(HappyMigration, migrations)

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, {})
    Q_WRITABLE_VAR_PROPERTY(int, port, Port, 32768)
    Q_WRITABLE_VAR_PROPERTY(bool, external, External, false)
    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)

    Q_CONSTANT_PTR_PROPERTY(QHttpServer, httpServer)
    Q_CONSTANT_OLP_PROPERTY(HappyRouter, routers)
    Q_DEFAULT_PROPERTY(routers)

public:
    explicit HappyServer(QObject *parent = nullptr);
    ~HappyServer();

    bool init();
    bool start();

    bool registerRouter(HappyRouter* router);

    Q_INVOKABLE HappyCrudRouter* crudRouter(const QString& tableName) const;
    Q_INVOKABLE QList<QWebSocket*> sockets(const QString& path, bool startsWith=false) const;

signals:
    void started(bool status);

    void socketConnected(const QString& path, QWebSocket* socket);
    void socketDisconnected(const QString& path, QWebSocket* socket);

protected slots:
    void onNewSocketConnection();
    void onSocketTextMessageReceived(const QString& message);
    void onSocketBinaryMessageReceived(const QByteArray& message);
    void onSocketDisconnected();

protected:
    virtual QHttpServerResponse missingHandler(const QHttpServerRequest& request);

    QHash<QString, HappyCrudRouter*> m_crudsRouterMap;

    QMultiHash<QString, QWebSocket*> m_sockets;
};

#endif // HAPPYSERVER_H
