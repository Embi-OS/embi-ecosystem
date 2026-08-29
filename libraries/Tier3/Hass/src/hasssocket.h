#ifndef HASSSOCKET_H
#define HASSSOCKET_H

#include <restsocket.h>

class HassSocket: public RestSocket
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, accessToken, AccessToken, {})
    Q_READONLY_VAR_PROPERTY(bool, authenticated, Authenticated, false)
    Q_READONLY_VAR_PROPERTY(bool, subscribed, Subscribed, false)
    Q_READONLY_REF_PROPERTY(QString, protocolError, ProtocolError, {})

public:
    explicit HassSocket(QObject* parent=nullptr);

signals:
    void authenticationFailed(const QString& error);
    void entityStateChanged(const QString& entityId, const QVariantMap& state);

private slots:
    void onAccessTokenChanged(const QString& accessToken);

    void resetProtocol();
    void handleTextMessage(const QString& message);

private:
    bool sendProtocolMessage(const QVariantMap& message);
    bool subscribeToStateChanges();
    void failAuthentication(const QString& error);

    static constexpr int SubscriptionId = 1;
};

#endif // HASSSOCKET_H
