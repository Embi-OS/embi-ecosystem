#ifndef NETWORKSETTINGSSERVICE_H
#define NETWORKSETTINGSSERVICE_H

#include <QDefs>

#include "networksettings.h"

class NetworkSettingsServicePrivate;
class NetworkSettingsService : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString id READ id CONSTANT FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool connected READ connected NOTIFY stateChanged FINAL)
    Q_PROPERTY(NetworkSettingsState::State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(NetworkSettingsType::Type type READ type NOTIFY typeChanged FINAL)
    Q_PROPERTY(NetworkSettingsIPv4* ipv4 READ ipv4 NOTIFY ipv4Changed FINAL)
    Q_PROPERTY(NetworkSettingsIPv6* ipv6 READ ipv6 NOTIFY ipv6Changed FINAL)
    Q_PROPERTY(NetworkSettingsProxy* proxy READ proxy  NOTIFY proxyChanged FINAL)
    Q_PROPERTY(NetworkSettingsEthernet* ethernetConfig READ ethernetConfig NOTIFY ethernetChanged FINAL)
    Q_PROPERTY(NetworkSettingsWireless* wirelessConfig READ wirelessConfig NOTIFY wirelessChanged FINAL)
    Q_PROPERTY(QAbstractItemModel* domains READ domains NOTIFY domainsChanged FINAL)
    Q_PROPERTY(QAbstractItemModel* nameservers READ nameservers NOTIFY nameserversChanged FINAL)
    Q_PROPERTY(QAbstractItemModel* timeservers READ timeservers NOTIFY timeserversChanged FINAL)
    Q_PROPERTY(bool autoConnect READ autoConnect WRITE setAutoConnect NOTIFY autoConnectChanged FINAL)
public:
    explicit NetworkSettingsService(const QString& aServiceId, QObject* parent = nullptr);

    QString id() const;
    QString name() const;
    bool connected();
    NetworkSettingsState::State state();
    NetworkSettingsType::Type type();
    NetworkSettingsIPv4* ipv4();
    NetworkSettingsIPv6* ipv6();
    NetworkSettingsProxy* proxy();
    QAbstractItemModel* domains();
    QAbstractItemModel* nameservers();
    QAbstractItemModel* timeservers();
    NetworkSettingsEthernet* ethernetConfig();
    NetworkSettingsWireless* wirelessConfig();
    void setPlaceholderState(bool placeholderState);
    bool placeholderState() const;

    Q_INVOKABLE void setAutoConnect(bool autoconnect);
    Q_INVOKABLE bool autoConnect() const;
    Q_INVOKABLE void setupIpv4Config();
    Q_INVOKABLE void setupIpv6Config();
    Q_INVOKABLE void setupTimeserversConfig();
    Q_INVOKABLE void setupNameserversConfig();
    Q_INVOKABLE void setupDomainsConfig();
    Q_INVOKABLE void setupNetworkSettingsProxy();
    //Wireless config
    Q_INVOKABLE void connectService();
    void doConnectService();
    Q_INVOKABLE void disconnectService();
    Q_INVOKABLE void removeService();
Q_SIGNALS:
    void autoConnectChanged();
    void nameChanged();
    void stateChanged();
    void typeChanged();
    void proxyChanged();
    void ipv4Changed();
    void ipv6Changed();
    void domainsChanged();
    void nameserversChanged();
    void timeserversChanged();
    void ethernetChanged();
    void wirelessChanged();
    void connectionStateCleared();
    void serviceConnected(NetworkSettingsService* service);
    void serviceDisconnected(NetworkSettingsService* service);
protected:
    NetworkSettingsServicePrivate *d_ptr;

    Q_DISABLE_COPY(NetworkSettingsService)
    Q_DECLARE_PRIVATE(NetworkSettingsService)
};

#endif // NETWORKSETTINGSSERVICE_H
