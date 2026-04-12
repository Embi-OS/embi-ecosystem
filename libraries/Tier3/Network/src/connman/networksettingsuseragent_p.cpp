#include <QDBusConnection>

#include "networksettingsuseragent.h"
#include "networksettingsuseragent_p.h"
#include "connmancommon.h"

const QString PropertyPassphrase(QStringLiteral("Passphrase"));

NetworkSettingsUserAgentPrivate::NetworkSettingsUserAgentPrivate(NetworkSettingsUserAgent *parent)
    :QDBusAbstractAdaptor(parent)
    ,q_ptr(parent)
    ,m_pendingReply(false)
{

    setAutoRelaySignals(true);
    QMetaObject::invokeMethod(this, "registerAgent", Qt::QueuedConnection);
}

void NetworkSettingsUserAgentPrivate::cancel()
{
    // handle method call net.connman.Agent.Cancel
    if (!m_pendingReply)
        return;

    m_pendingReply = false;
    QDBusMessage errorMessage = m_pendingMessage.createErrorReply(
                              QStringLiteral("net.connman.Agent.Error.Canceled"),
                              QStringLiteral(""));
    QDBusConnection::systemBus().send(errorMessage);
}

void NetworkSettingsUserAgentPrivate::release()
{
    // handle method call net.connman.Agent.Release
    QMetaObject::invokeMethod(parent(), "Release");
}

void NetworkSettingsUserAgentPrivate::ReportError(const QDBusObjectPath &path, const QString &param)
{
    Q_Q(NetworkSettingsUserAgent);
    Q_UNUSED(path);
    Q_UNUSED(param);
    if (!m_ssid.isEmpty())
        q->requestNextConnection();
    else
        emit q->error();
}

void NetworkSettingsUserAgentPrivate::registerAgent()
{
    Q_Q(NetworkSettingsUserAgent);
    QDBusConnection::systemBus().registerObject(Q_PropertyAgentPath, q);
}

QVariantMap NetworkSettingsUserAgentPrivate::RequestInput(const QDBusObjectPath &path, const QVariantMap &params, const QDBusMessage &msg)
{
    Q_Q(NetworkSettingsUserAgent);
    Q_UNUSED(path);
    QVariant name = params[Q_PropertyName];
    if (!name.isValid()) {
        m_ssid.clear();
    }
    QVariantMap response;
    QVariant passPhrase = params[PropertyPassphrase];
    if (name.isValid() && !m_ssid.isEmpty()) {
        response[Q_PropertyName] = m_ssid;
    }
    if (passPhrase.isValid()) {
        if (!m_passphrase.isEmpty()) {
            response[PropertyPassphrase] = m_passphrase;
        } else {
            msg.setDelayedReply(true);
            m_pendingMessage = msg;
            m_pendingReply = true;
            emit q->showUserCredentialsInput();
            return QVariantMap();
        }
    }
    return response;
}

void NetworkSettingsUserAgentPrivate::setPassphrase(const QString& passphrase)
{
    m_passphrase = passphrase;
    if (m_pendingReply) {
        QVariantMap response;
        if (!m_ssid.isEmpty()) {
            response[Q_PropertyName] = m_ssid;
        }
        response[PropertyPassphrase] = m_passphrase;
        QDBusMessage reply = m_pendingMessage.createReply();
        reply<<response;
        m_pendingReply = false;
        QDBusConnection::systemBus().send(reply);
    }
}

void NetworkSettingsUserAgentPrivate::setSsidAndPassphrase(const QString &ssid, const QString &passphrase)
{
    m_ssid = ssid;
    m_passphrase = passphrase;
}

void NetworkSettingsUserAgentPrivate::clearConnectionState()
{
    m_passphrase.clear();
    m_ssid.clear();
}
