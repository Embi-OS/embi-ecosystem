#include "networksettingsservice_p.h"
#include "connman_service_interface.h"
#include "connmancommon.h"
#include <QHostAddress>

const QString PropertyIPv4(QStringLiteral("IPv4"));
const QString PropertyQNetworkSettingsIPv4(QStringLiteral("IPv4.Configuration"));
const QString PropertyIPv6(QStringLiteral("IPv6"));
const QString PropertyQNetworkSettingsIPv6(QStringLiteral("IPv6.Configuration"));
const QString PropertyTimeservers(QStringLiteral("Timeservers"));
const QString PropertyTimeserversConfig(QStringLiteral("Timeservers.Configuration"));
const QString PropertyNameservers(QStringLiteral("Nameservers"));
const QString PropertyNameserversConfig(QStringLiteral("Nameservers.Configuration"));
const QString PropertyDomains(QStringLiteral("Domains"));
const QString PropertyDomainsConfig(QStringLiteral("Domains.Configuration"));
const QString PropertyProxy(QStringLiteral("Proxy"));
const QString PropertyQNetworkSettingsProxy(QStringLiteral("Proxy.Configuration"));
const QString PropertyAddress(QStringLiteral("Address"));
const QString PropertyNetMask(QStringLiteral("Netmask"));
const QString PropertyGateway(QStringLiteral("Gateway"));
const QString PropertyPrefixLength(QStringLiteral("PrefixLength"));
const QString PropertyMethod(QStringLiteral("Method"));
const QString PropertyPrivacy(QStringLiteral("Privacy"));
const QString PropertyUrl(QStringLiteral("URL"));
const QString PropertyServers(QStringLiteral("Servers"));
const QString PropertyExcludes(QStringLiteral("Excludes"));
const QString PropertyStrength(QStringLiteral("Strength"));
const QString PropertySecurity(QStringLiteral("Security"));
const QString PropertyEthernet(QStringLiteral("Ethernet"));
const QString PropertyInterface(QStringLiteral("Interface"));

const QString AttributeAuto(QStringLiteral("auto"));
const QString AttributeDhcp(QStringLiteral("dhcp"));
const QString AttributeManual(QStringLiteral("manual"));
const QString AttributeOff(QStringLiteral("off"));
const QString AttributeDisabled(QStringLiteral("disabled"));
const QString AttributeEnabled(QStringLiteral("enabled"));
const QString AttributePreferred(QStringLiteral("preferred"));
const QString AttributeDirect(QStringLiteral("direct"));
const QString AttributeNone(QStringLiteral("none"));
const QString AttributeWep(QStringLiteral("wep"));
const QString AttributePsk(QStringLiteral("psk"));
const QString AttributeIeee(QStringLiteral("ieee8021x"));
const QString AttributeWps(QStringLiteral("wps"));
const QString AttributeInvalidKey(QStringLiteral("invalid-key"));

const QVariantMap &operator>>(const QVariantMap &argument, NetworkSettingsIPv4 &obj)
{
    obj.setAddress(argument[PropertyAddress].toString());
    obj.setMask(argument[PropertyNetMask].toString());
    obj.setGateway(argument[PropertyGateway].toString());

    QString method = argument[PropertyMethod].toString();

    if (method == AttributeDhcp) {
        obj.setMethod(NetworkSettingsIPv4::Dhcp);
    }
    else if (method == AttributeManual) {
        obj.setMethod(NetworkSettingsIPv4::Manual);
    }
    else if (method == AttributeOff) {
        obj.setMethod(NetworkSettingsIPv4::Off);
    }
    else {
        obj.setMethod(NetworkSettingsIPv4::Off);
    }
    return argument;
}

const QVariantMap &operator<<(QVariantMap &argument, const NetworkSettingsIPv4 &obj)
{
    argument.insert(PropertyAddress, QVariant::fromValue(obj.getAddress()));

    argument.insert(PropertyNetMask, QVariant::fromValue(obj.getMask()));

    argument.insert(PropertyGateway, QVariant::fromValue(obj.getGateway()));

    QString method;
    switch (obj.getMethod()) {
    case NetworkSettingsIPv4::Dhcp:
        method = AttributeDhcp;
        break;
    case NetworkSettingsIPv4::Manual:
        method = AttributeManual;
        break;
    case NetworkSettingsIPv4::Off:
        method = AttributeOff;
        break;
    default:
        break;
    }
    argument.insert(PropertyMethod, QVariant::fromValue(method));
    return argument;
}

const QVariantMap &operator>>(const QVariantMap &argument, NetworkSettingsIPv6 &obj)
{
    obj.setAddress(argument[PropertyAddress].toString());
    obj.setGateway(argument[PropertyGateway].toString());
    obj.setPrefixLength(argument[PropertyPrefixLength].toInt());
    QString method = argument[PropertyMethod].toString();
    if (method == AttributeAuto) {
        obj.setMethod(NetworkSettingsIPv6::Auto);
    }
    else if (method == AttributeManual) {
        obj.setMethod(NetworkSettingsIPv6::Manual);
    }
    else if (method == AttributeOff) {
        obj.setMethod(NetworkSettingsIPv6::Off);
    }
    QString privacy = argument[PropertyPrivacy].toString();

    if (privacy == AttributeDisabled) {
        obj.setPrivacy(NetworkSettingsIPv6::Disabled);
    }
    else if (privacy == AttributeEnabled) {
        obj.setPrivacy(NetworkSettingsIPv6::Enabled);
    }
    else if (privacy == AttributePreferred) {
        obj.setPrivacy(NetworkSettingsIPv6::Preferred);
    }
    return argument;
}


const QVariantMap &operator<<(QVariantMap &argument, const NetworkSettingsIPv6 &obj)
{
    argument.insert(PropertyAddress, QVariant::fromValue(obj.getAddress()));
    argument.insert(PropertyGateway, QVariant::fromValue(obj.getGateway()));
    argument.insert(PropertyPrefixLength, QVariant::fromValue(obj.getPrefixLength()));

    QString method;
    switch (obj.getMethod()) {
    case NetworkSettingsIPv6::Auto:
        method = AttributeAuto;
        break;
    case NetworkSettingsIPv6::Manual:
        method = AttributeManual;
        break;
    case NetworkSettingsIPv6::Off:
        method = AttributeOff;
        break;
    default:
        break;
    }
    argument.insert(PropertyMethod, QVariant::fromValue(method));

    QString privacy;
    switch (obj.getPrivacy()) {
    case NetworkSettingsIPv6::Disabled:
        method = AttributeDisabled;
        break;
    case NetworkSettingsIPv6::Enabled:
        method = AttributeEnabled;
        break;
    case NetworkSettingsIPv6::Preferred:
        method = AttributePreferred;
        break;
    default:
        break;
    }
    argument.insert(PropertyPrivacy, QVariant::fromValue(privacy));

    return argument;
}


const QVariantMap &operator>>(const QVariantMap &argument, NetworkSettingsProxy &obj)
{
    QString method = argument[PropertyMethod].toString();
    if (method == AttributeDirect) {
        obj.setMethod(NetworkSettingsProxy::Direct);
    }
    else if (method == AttributeAuto) {
        obj.setMethod(NetworkSettingsProxy::Auto);
    }
    else if (method == AttributeManual) {
        obj.setMethod(NetworkSettingsProxy::Manual);
    }

    QString urlStr = argument[PropertyUrl].toString();
    obj.setUrl(QUrl(urlStr));

    obj.setServers(argument[PropertyServers].toStringList());
    obj.setExcludes(argument[PropertyExcludes].toStringList());

    return argument;
}

const QVariantMap &operator<<(QVariantMap &argument, const NetworkSettingsProxy &obj)
{
    QString method;
    switch (obj.method()) {
    case NetworkSettingsProxy::Direct:
        method = AttributeDirect;
        break;
    case NetworkSettingsProxy::Auto:
        method = AttributeAuto;
        break;
    case NetworkSettingsProxy::Manual:
        method = AttributeManual;
        break;
    default:
        break;
    }
    argument.insert(PropertyMethod, QVariant::fromValue(method));
    if (method == AttributeAuto) {
        argument.insert(PropertyUrl, QVariant::fromValue(obj.url().toString()));
    } else if (method == AttributeManual) {
        argument.insert(PropertyServers, QVariant::fromValue(obj.servers()));
        argument.insert(PropertyExcludes, QVariant::fromValue(obj.excludes()));
    }
    return argument;
}

const QVariantMap &operator>>(const QVariantMap &argument, NetworkSettingsEthernet &obj)
{
    obj.setAddress(argument[PropertyAddress].toString());
    obj.setInterface(argument[PropertyInterface].toString());

    QString method = argument[PropertyMethod].toString();

    if (method == AttributeAuto) {
        obj.setMethod(NetworkSettingsEthernet::Auto);
    }
    else if (method == AttributeManual) {
        obj.setMethod(NetworkSettingsEthernet::Manual);
    }
    else if (method == AttributeOff) {
        obj.setMethod(NetworkSettingsEthernet::Off);
    }
    else {
        obj.setMethod(NetworkSettingsEthernet::Off);
    }
    return argument;
}

NetworkSettingsServicePrivate::NetworkSettingsServicePrivate(const QString& id, NetworkSettingsService *parent) :
    QObject(parent)
    ,q_ptr(parent)
    ,m_id(id)
{
    qDBusRegisterMetaType<ConnmanMapStruct>();
    qDBusRegisterMetaType<ConnmanMapStructList>();

    m_service = new NetConnmanServiceInterface(QStringLiteral("net.connman"), m_id,
                                                  QDBusConnection::systemBus(), this);

    connect(m_service, SIGNAL(PropertyChanged(QString,QDBusVariant)),
            this, SLOT(updateProperty(QString,QDBusVariant)));

    QDBusPendingReply<QVariantMap> reply = m_service->GetProperties();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(propertiesUpdated(QDBusPendingCallWatcher*)));
}

void NetworkSettingsServicePrivate::setAutoConnect(bool autoconnect)
{
    if (m_service) {
        m_service->SetProperty(Q_PropertyAutoConnect, QDBusVariant(QVariant(autoconnect)));
        m_autoConnect = autoconnect;
    }
}

bool NetworkSettingsServicePrivate::autoConnect() const
{
    return m_autoConnect;
}

void NetworkSettingsServicePrivate::setupIpv6Config()
{
    QVariantMap param;
    param<<m_ipv6config;
    if (m_service)
        m_service->SetProperty(PropertyQNetworkSettingsIPv6, QDBusVariant(QVariant(param)));
}

void NetworkSettingsServicePrivate::setupTimeserversConfig()
{
    QVariant param = QVariant::fromValue(m_timeserverConfig.stringList());
    if (m_service)
        m_service->SetProperty(PropertyTimeserversConfig, QDBusVariant(QVariant(param)));
}

void NetworkSettingsServicePrivate::setupNameserversConfig()
{
    QVariant param = QVariant::fromValue(m_nameserverConfig.stringList());
    if (m_service)
        m_service->SetProperty(PropertyNameserversConfig, QDBusVariant(QVariant(param)));
}

void NetworkSettingsServicePrivate::setupDomainsConfig()
{
    QVariant param = QVariant::fromValue(m_domainsConfig.stringList());
    if (m_service)
        m_service->SetProperty(PropertyDomainsConfig, QDBusVariant(QVariant(param)));
}

void NetworkSettingsServicePrivate::setupQNetworkSettingsProxy()
{
    QVariantMap param;
    param<<m_proxyConfig;
    if (m_service)
        m_service->SetProperty(PropertyQNetworkSettingsProxy, QDBusVariant(QVariant(param)));
}

static void ensureMaskAndGateway(NetworkSettingsIPv4& ipv4Settings)
{
    if (ipv4Settings.getMask().isEmpty()) {
        ipv4Settings.setMask(QString::fromUtf8("255.255.0.0"));
    }
    if (ipv4Settings.getGateway().isEmpty()) {
        QHostAddress addr{ipv4Settings.getAddress()};
        quint32 addrIp  = addr.toIPv4Address();
        QHostAddress mask{ipv4Settings.getMask()};
        quint32 maskIp  = mask.toIPv4Address();
        quint32 gwIp = addrIp & maskIp;
        ipv4Settings.setGateway(QHostAddress{gwIp}.toString());
    }
}

void NetworkSettingsServicePrivate::setupIpv4Config()
{
    ensureMaskAndGateway(m_ipv4config);
    QVariantMap param;
    param<<m_ipv4config;
    if (m_service)
        m_service->SetProperty(PropertyQNetworkSettingsIPv4, QDBusVariant(QVariant(param)));
}

void NetworkSettingsServicePrivate::connectService()
{
    if (m_service) {
        m_service->Connect();
    }
}

void NetworkSettingsServicePrivate::disconnectService()
{
    if (m_service)
        m_service->Disconnect();

}

void NetworkSettingsServicePrivate::removeService()
{
    if (m_service)
        m_service->Remove();
}

void NetworkSettingsServicePrivate::setPlaceholderState(bool placeholderState)
{
    m_placeholderState = placeholderState;
}

bool NetworkSettingsServicePrivate::placeholderState() const
{
    return m_placeholderState;
}

void NetworkSettingsServicePrivate::propertiesUpdated(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;
    call->deleteLater();

    if (!reply.isError())
        setupConfiguration(reply.value());
}

void NetworkSettingsServicePrivate::updateProperty(const QString &name, const QDBusVariant &value)
{
    updateProperty(name, value.variant());
}

void NetworkSettingsServicePrivate::updateProperty(const QString& key, const QVariant& val)
{
    Q_Q(NetworkSettingsService);

    if (key == PropertyQNetworkSettingsIPv4) {
        QVariantMap value = qdbus_cast<QVariantMap>(val);
        if (value[PropertyMethod].toString() == AttributeManual) {
            value >> m_ipv4config;
            emit q->ipv4Changed();
        }
    }
    else if (key == PropertyIPv4) {
        QVariantMap value = qdbus_cast<QVariantMap>(val);
        if (value[PropertyMethod].toString() != AttributeManual) {
            value >> m_ipv4config;
            emit q->ipv4Changed();
        }
    }
    else if (key == PropertyIPv6) {
        QVariantMap value = qdbus_cast<QVariantMap>(val);
        value >> m_ipv6config;
        emit q->ipv6Changed();
    }
    else if (key == PropertyTimeservers) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_timeserverConfig.setStringList(value);
        emit q->timeserversChanged();
    }
    else if (key == PropertyTimeserversConfig) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_timeserverConfig.setAutomatic(value.isEmpty());
        emit q->timeserversChanged();
    }
    else if (key == PropertyNameservers) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_nameserverConfig.setStringList(value);
        emit q->nameserversChanged();
    }
    else if (key == PropertyNameserversConfig) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_nameserverConfig.setAutomatic(value.isEmpty());
        emit q->nameserversChanged();
    }
    else if (key == PropertyDomains) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_domainsConfig.setStringList(value);
        emit q->domainsChanged();
    }
    else if (key == PropertyDomainsConfig) {
        QStringList value = qdbus_cast<QStringList>(val);
        m_domainsConfig.setAutomatic(value.isEmpty());
        emit q->domainsChanged();
    }
    else if (key == PropertyProxy) {
        QVariantMap value = qdbus_cast<QVariantMap>(val);
        value >> m_proxyConfig;
        emit q->proxyChanged();
    }
    else if (key == Q_PropertyState) {
        QString value = qdbus_cast<QString>(val);
        NetworkSettingsState oldState;
        oldState.setState(m_state.state());
        value >> m_state;
        emit q->stateChanged();
        if (m_state.state() == NetworkSettingsState::Ready || m_state.state() == NetworkSettingsState::Online)
            emit q->serviceConnected(q);
        if (m_state.state() == NetworkSettingsState::Disconnect)
            emit q->serviceDisconnected(q);
    }
    else if (key == Q_PropertyName) {
        m_name = qdbus_cast<QString>(val);
        emit q->nameChanged();
    }
    else if (key == Q_PropertyType) {
        QString value = qdbus_cast<QString>(val);
        value >> m_type;
        emit q->typeChanged();
        if (m_state.state() == NetworkSettingsState::Ready || m_state.state() == NetworkSettingsState::Online)
            emit q->serviceConnected(q);
        if (m_state.state() == NetworkSettingsState::Disconnect)
            emit q->serviceDisconnected(q);
    }
    else if (key == PropertyEthernet) {
        QVariantMap value = qdbus_cast<QVariantMap>(val);
        value >> m_ethernetConfig;
        emit q->ethernetChanged();
    }
    else if (key == PropertyStrength) {
        m_wifiConfig.setSignalStrength(val.toInt());
    }
    else if (key == PropertySecurity) {
        QStringList value = qdbus_cast<QStringList>(val);
        NetworkSettingsWireless::Securities securities;
        for (const QString& str: value) {
            if (str ==  AttributeNone || str == AttributeWps) {
                securities |= NetworkSettingsWireless::Security::None;
            }
            else if (str == AttributeWep) {
                securities |= NetworkSettingsWireless::Security::WEP;
            }
            else if (str == AttributePsk) {
                securities |=NetworkSettingsWireless::Security::WPA;
            }
            else if (str == AttributeIeee) {
                securities |=NetworkSettingsWireless::Security::WPA2;
            }
        }
        m_wifiConfig.setSecurity(securities);
    }
    else if (key == Q_PropertyAutoConnect) {
        m_autoConnect = qdbus_cast<bool>(val);
        emit q->autoConnectChanged();
    }
}

void NetworkSettingsServicePrivate::setupConfiguration(const QVariantMap &properties)
{
    QVariantMap::const_iterator it = properties.constBegin(), end = properties.constEnd();
    for ( ; it != end; ++it) {
       updateProperty(it.key(), it.value());
    }
}
