#ifndef RESTSOCKET_H
#define RESTSOCKET_H

#include <QDefs>
#include <QWebSocket>

#include "rest_helpertypes.h"

Q_ENUM_CLASS(RestSocketStates, RestSocketState,
             Connecting  = 0,
             Open        = 1,
             Closing     = 2,
             Closed      = 3,
             Error       = 4)

class RestSocket: public QObject,
                  public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, RestHelper::defaultConnection)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_REF_PROPERTY(QString, method, Method, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, parameters, Parameters, {})
    Q_WRITABLE_VAR_PROPERTY(bool, bindWhen, BindWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, reconnect, Reconnect, true)

    Q_WRITABLE_REF_PROPERTY(QStringList, requestedProtocols, RequestedProtocols, {})

    Q_READONLY_REF_PROPERTY(QString, url, Url, {})
    Q_READONLY_VAR_PROPERTY(RestSocketStates::Enum, status, Status, RestSocketStates::Closed)
    Q_READONLY_VAR_PROPERTY(bool, connected, Connected, false)
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})
    Q_READONLY_REF_PROPERTY(QString, negotiatedProtocol, NegotiatedProtocol, {})

public:
    explicit RestSocket(QObject *parent=nullptr);
    explicit RestSocket(const QString& path, QObject *parent=nullptr);
    virtual ~RestSocket();

    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    static void setGloballyEnabled(bool globallyEnabled);
    static bool globallyEnabled();

public slots:
    virtual void queueBind();
    virtual void bind();
    virtual bool waitForBind(int timeout=-1);

    virtual void unbind();

    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &message);
    qint64 sendMessage(const QVariant &message);

    void ping(const QByteArray &payload = QByteArray());

signals:
    void completedChanged(bool completed);

    void textMessageReceived(const QString& message);
    void binaryMessageReceived(const QByteArray& message);
    void messageReceived(const QVariant& message);

    void connected();
    void disconnected();
    void error();
    void sslErrors(const QList<QSslError> &errors, bool ignoreErrors);

    void pong(quint64 elapsedTime, const QByteArray &payload);

protected slots:
    void handleTextMessageReceived(const QString& message);
    void handleBinaryMessageReceived(const QByteArray& message);
    void handleError(QAbstractSocket::SocketError error);
    void handleSslErrors(const QList<QSslError> &errors);
    void handleStateChanged(QAbstractSocket::SocketState state);

    void open();
    void close();
    void closeGoingAway();

private:
    QWebSocket* m_socket;

    bool m_completed = false;
    bool m_bindQueued = false;

    RestDataMode m_dataMode;
};

#endif // RESTSOCKET_H
