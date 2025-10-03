#ifndef NETWORKSETTINGSSERVICEPRIVATE_H
#define NETWORKSETTINGSSERVICEPRIVATE_H

#include "networksettingsservice.h"
#include "networksettings.h"

// Automatically generated class in global namespace
class NetConnmanServiceInterface;

class QDBusVariant;
class QDBusPendingCallWatcher;
class NetworkSettingsServicePrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(NetworkSettingsService)
public:
    NetworkSettingsServicePrivate(const QString& aPath, NetworkSettingsService *parent = nullptr);

    NetworkSettingsService *q_ptr;
private slots:
    void propertiesUpdated(QDBusPendingCallWatcher *call);
    void updateProperty(const QString &name, const QDBusVariant &value);

private:
    void setupConfiguration(const QVariantMap &properties);
    void updateProperty(const QString& key, const QVariant& value);

protected:
    void setAutoConnect(bool autoconnect);
    bool autoConnect() const;
    void setupIpv4Config();
    void setupIpv6Config();
    void setupTimeserversConfig();
    void setupNameserversConfig();
    void setupDomainsConfig();
    void setupQNetworkSettingsProxy();
    void connectService();
    void disconnectService();
    void removeService();
    void setPlaceholderState(bool placeholderState);
    bool placeholderState() const;

    QString m_id;
    QString m_name;
    NetworkSettingsState m_state;
    NetworkSettingsIPv4 m_ipv4config;
    NetworkSettingsIPv6 m_ipv6config;
    NetworkSettingsAddressModel m_domainsConfig;
    NetworkSettingsAddressModel m_nameserverConfig;
    NetworkSettingsAddressModel m_timeserverConfig;
    NetworkSettingsProxy m_proxyConfig;
    NetworkSettingsWireless m_wifiConfig;
    NetworkSettingsEthernet m_ethernetConfig;
    NetworkSettingsType m_type;
    NetConnmanServiceInterface *m_service;
    bool m_placeholderState = false;
    bool m_autoConnect = false;
};

#endif // NETWORKSETTINGSSERVICEPRIVATE_H
