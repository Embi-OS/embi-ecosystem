#include "hasssocket.h"
#include "hass_log.h"

HassSocket::HassSocket(QObject* parent):
    RestSocket("api/websocket", parent)
{
    setBindWhen(false);

    connect(this, &RestSocket::connected, this, &HassSocket::resetProtocol);
    connect(this, &RestSocket::disconnected, this, [this]() {
        setAuthenticated(false);
        setSubscribed(false);
    });
    connect(this, &RestSocket::textMessageReceived, this, &HassSocket::handleTextMessage);
    connect(this, &HassSocket::accessTokenChanged, this, &HassSocket::onAccessTokenChanged);
}

void HassSocket::onAccessTokenChanged(const QString& accessToken)
{
    resetProtocol();
    setBindWhen(!accessToken.isEmpty());
    if(getConnected())
        reconnectNow("Home Assistant access token changed");
}

void HassSocket::resetProtocol()
{
    resetAuthenticated();
    resetSubscribed();
    resetProtocolError();
}

void HassSocket::handleTextMessage(const QString& message)
{
    QJsonParseError parseError;
    const QVariant payloadData = QUtils::Json::jsonToVariant(message.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError
        || payloadData.metaType().id() != QMetaType::QVariantMap) {
        setProtocolError(QString("Invalid Home Assistant WebSocket message: %1").arg(parseError.errorString()));
        return;
    }

    const QVariantMap payload = payloadData.toMap();
    const QString type = payload.value("type").toString();

    if(type == "auth_required") {
        if(m_accessToken.isEmpty()) {
            failAuthentication("Home Assistant access token is missing");
            return;
        }

        QVariantMap message;
        message.insert("type", "auth");
        message.insert("access_token", m_accessToken);
        if(!sendProtocolMessage(message))
            failAuthentication("Unable to send Home Assistant authentication");
        return;
    }

    if(type == "auth_ok") {
        HASSLOG_INFO()<<"Synchronisation active";

        setAuthenticated(true);
        subscribeToStateChanges();
        return;
    }

    if(type == "auth_invalid") {
        failAuthentication(payload.value("message", "Home Assistant authentication was rejected").toString());
        return;
    }

    if(type == "result") {
        if(payload.value("id").toInt() != SubscriptionId)
            return;

        if(payload.value("success").toBool()) {
            setSubscribed(true);
            resetProtocolError();
        }
        else {
            const QString error = payload.value("error").toMap().value("message", "Home Assistant subscription was rejected").toString();
            setProtocolError(error);
        }
        return;
    }

    if(type != "event" || payload.value("id").toInt() != SubscriptionId)
        return;

    const QVariantMap event = payload.value("event").toMap();
    if(event.value("event_type").toString() != "state_changed")
        return;

    const QVariantMap data = event.value("data").toMap();
    const QString entityId = data.value("entity_id").toString();
    if(!entityId.isEmpty())
        emit this->entityStateChanged(entityId, data.value("new_state").toMap());
}

bool HassSocket::sendProtocolMessage(const QVariantMap& message)
{
    const QString payload = QString::fromUtf8(QUtils::Json::variantToJson(message));
    return sendTextMessage(payload) > 0;
}

bool HassSocket::subscribeToStateChanges()
{
    QVariantMap message;
    message.insert("id", SubscriptionId);
    message.insert("type", "subscribe_events");
    message.insert("event_type", "state_changed");

    if(sendProtocolMessage(message))
        return true;

    setSubscribed(false);
    setProtocolError("Unable to subscribe to Home Assistant state changes");
    return false;
}

void HassSocket::failAuthentication(const QString& error)
{
    setAuthenticated(false);
    setSubscribed(false);
    setProtocolError(error);

    HASSLOG_WARNING()<<"Failed authentication"<<error;

    emit this->authenticationFailed(error);
    setBindWhen(false);
}
