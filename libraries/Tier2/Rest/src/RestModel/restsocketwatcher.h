#ifndef RESTSOCKETWATCHER_H
#define RESTSOCKETWATCHER_H

#include "restsocket.h"
class RestSocketWatcher: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    QML_ATTACHED(RestSocketWatcher)

    Q_OBJECT_ATTACHED(RestSocketWatcher, RestSocket)

    Q_CONSTANT_PTR_PROPERTY(RestSocket, socket)
    Q_READONLY_VAR_PROPERTY(bool, connected, Connected, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(int, pingIntervalMs, PingIntervalMs, 5000)
    Q_WRITABLE_VAR_PROPERTY(int, pongTimeoutMs, PongTimeoutMs, 5000)
    Q_WRITABLE_VAR_PROPERTY(int, maxPingTimeoutCount, MaxPingTimeoutCount, 5)
    Q_WRITABLE_VAR_PROPERTY(bool, reconnectOnPingTimeout, ReconnectOnPingTimeout, true)

protected:
    explicit RestSocketWatcher(RestSocket* parent);

signals:
    void pongReceived(quint64 elapsedTime);
    void connectionLost(const QString& reason);

private slots:
    void onPongReceived(quint64 elapsedTime, const QByteArray &payload);
    void onPingTimeout();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketStatusChanged(RestSocketStates::Enum status);

    void queueSendPing();
    void sendPing();

private:
    void resetPingState();
    void applyTimersConfiguration();
    void handleSocketDisconnected();
    void setDisconnected(const QString& reason);

    QByteArray m_lastPingPayload;
    QTimer* m_pingCaller=nullptr;
    QTimer* m_pingWatchdog=nullptr;
    int m_pingTimeoutCount=0;
};

#endif // RESTSOCKETWATCHER_H
