#ifndef NETWORKSETTINGSUSERAGENTPRIVATE_H
#define NETWORKSETTINGSUSERAGENTPRIVATE_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusAbstractAdaptor>

class NetworkSettingsUserAgent;
class NetworkSettingsUserAgentPrivate : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "net.connman.Agent")
    Q_DECLARE_PUBLIC(NetworkSettingsUserAgent)
public:
    NetworkSettingsUserAgentPrivate(NetworkSettingsUserAgent *parent = nullptr);
    void setPassphrase(const QString &passphrase);
    QString passphrase() const {return m_passphrase;}
    void cancel();
    void release();
    void setSsidAndPassphrase(const QString &ssid, const QString &passphrase);
    void clearConnectionState();
public Q_SLOTS: // Dbus methods
    void ReportError(const QDBusObjectPath &path, const QString &param);
    QVariantMap RequestInput(const QDBusObjectPath &path, const QVariantMap &params,
                             const QDBusMessage &message);
    void registerAgent();
private:
    NetworkSettingsUserAgent *q_ptr;
    QDBusMessage m_pendingMessage;
    bool m_pendingReply;
    QString m_passphrase;
    QString m_ssid;
};

#endif // NETWORKSETTINGSUSERAGENTPRIVATE_H

