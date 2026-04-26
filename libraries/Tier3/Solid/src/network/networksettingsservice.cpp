#include "networksettingsservice.h"
#include "networksettingsservice_p.h"

/*!
    \class NetworkSettingsService
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsService class represents a network service.

    \sa NetworkSettingsManager::services()
*/

/*!
    \property NetworkSettingsService::id
    \readonly
    \brief Holds a unique ID of this service.
*/

/*!
    \property NetworkSettingsService::name
    \readonly
    \brief Holds the name of this service.
*/


/*!
    \property NetworkSettingsService::state
    \readonly
    \brief Holds the state of this service.

    \sa NetworkSettingsState::State
*/

/*!
    \property NetworkSettingsService::type
    \readonly
    \brief Holds the type of this service.

    \sa NetworkSettingsType::Type
*/

/*!
    \property NetworkSettingsService::ipv4
    \readonly
    \brief Holds the IPv4 address for this service.
*/

/*!
    \property NetworkSettingsService::ipv6
    \readonly
    \brief Holds the IPv6 address for this service.
*/

/*!
    \property NetworkSettingsService::proxy
    \readonly
    \brief Holds the proxy settings for this service.
*/

/*!
    \property NetworkSettingsService::wirelessConfig
    \readonly
    \brief Holds the wireless configuration for this service.
*/

/*!
    \property NetworkSettingsService::domains
    \readonly
    \brief The model containing the domains associated with this service.

    This property can be used as a model for a view that lists the domain
    addresses associated with this service.
*/

/*!
    \property NetworkSettingsService::nameservers
    \readonly
    \brief The model containing the domain name servers associated with this
           service.

    This property can be used as a model for a view that lists the domain name
    server (DNS) addresses associated with this service.
*/

/*!
    \property NetworkSettingsService::autoConnect
    \brief Holds autoconnect property for this service.

    Autoconnect property determines whether the service connects automatically
    when it is available.
*/

/*!
    \fn NetworkSettingsService::connectionStateCleared()

    This signal is emitted when the connection state is cleared.
*/

/*!
    \fn NetworkSettingsService::serviceConnected(NetworkSettingsService* service);

    This signal is emitted when the connection to the network \a service is
    created.
*/

/*!
    \fn NetworkSettingsService::serviceDisconnected(NetworkSettingsService* service);

    This signal is emitted when the connection to the network \a service is
    cut.
*/

/*!
    \qmltype NetworkService
    \inqmlmodule QtDeviceUtilities.NetworkSettings
    \brief Represents a network service.

    The NetworkService QML type represents a network service.

    Instances of NetworkService cannot be created directly; instead, they can
    be retrieved via NetworkSettingsManager.

    \sa {NetworkSettingsManager::services}{NetworkSettingsManager.services}
*/

/*!
    Creates a network service with the identifier \a aServiceId and
    parent \a parent.
*/
NetworkSettingsService::NetworkSettingsService(const QString& aServiceId, QObject* parent) :
   QObject(parent)
  ,d_ptr(new NetworkSettingsServicePrivate(aServiceId, this))
{

}

/*!
    \qmlproperty string NetworkService::id
    \readonly
    \brief Holds a unique ID of this service.
*/

/*!
    Returns the unique identifier of the network service.
*/
QString NetworkSettingsService::id() const
{
    Q_D(const NetworkSettingsService);
    return d->m_id;
}

/*!
    \qmlproperty string NetworkService::name
    \readonly
    \brief Holds the name of this service.
*/

/*!
    Returns the network service name.
*/
QString NetworkSettingsService::name() const
{
    Q_D(const NetworkSettingsService);
    return d->m_name;
}

bool NetworkSettingsService::connected()
{
    return state() == NetworkSettingsState::Online || state() == NetworkSettingsState::Ready;
}

/*!
    \fn void NetworkSettingsService::setAutoConnect(bool autoconnect)

    Sets automatic connections to the network service to \a autoconnect.
*/
void NetworkSettingsService::setAutoConnect(const bool autoconnect)
{
    Q_D(NetworkSettingsService);
    d->setAutoConnect(autoconnect);
}

/*!
    Returns the automatic connection status of the network service.
*/
bool NetworkSettingsService::autoConnect() const
{
    Q_D(const NetworkSettingsService);
    return d->autoConnect();
}

/*!
    \qmlproperty enumeration NetworkService::state
    \readonly
    \brief Holds the state of this service.

    See \l [QML] {NetworkInterface::state}{NetworkInterface.state}
    for possible states.
*/

/*!
    Returns the network service state.

    \sa NetworkSettingsState::State
*/
NetworkSettingsState::State NetworkSettingsService::state()
{
    Q_D(NetworkSettingsService);
    return d->m_state.state();
}

/*!
    \qmlproperty enumeration NetworkService::type
    \readonly
    \brief Holds the type of this service.

    See \l [QML] {NetworkInterface::type}{NetworkInterface.type}
    for possible types.
*/

/*!
    Returns the network service type.

    \sa NetworkSettingsType::Type
*/
NetworkSettingsType::Type NetworkSettingsService::type()
{
    Q_D(NetworkSettingsService);
    return d->m_type.type();
}

/*!
    \qmlproperty NetworkSettingsIPv4 NetworkService::ipv4
    \readonly
    \brief Holds the IPv4 address for this service.
*/

/*!
    Returns the IPv4 address of the network service.
*/
NetworkSettingsIPv4* NetworkSettingsService::ipv4()
{
    Q_D(NetworkSettingsService);
    return &d->m_ipv4config;
}

/*!
    \qmlproperty NetworkSettingsIPv6 NetworkService::ipv6
    \readonly
    \brief Holds the IPv6 address for this service.
*/

/*!
    Returns the IPv6 address of the network service.
*/
NetworkSettingsIPv6* NetworkSettingsService::ipv6()
{
    Q_D(NetworkSettingsService);
    return &d->m_ipv6config;
}

/*!
    \qmlproperty NetworkSettingsProxy NetworkService::proxy
    \readonly
    \brief Holds the proxy settings for this service.
*/

/*!
    Returns the address of proxy used for the network service.
*/
NetworkSettingsProxy* NetworkSettingsService::proxy()
{
    Q_D(NetworkSettingsService);
    return &d->m_proxyConfig;
}

/*!
    \qmlproperty NetworkSettingsEthernet NetworkService::ethernetConfig
    \readonly
    \brief Holds the ethernet configuration for this service.
*/

/*!
    Returns the ethernet configuration of the network service.
*/
NetworkSettingsEthernet* NetworkSettingsService::ethernetConfig()
{
    Q_D(NetworkSettingsService);
    return &d->m_ethernetConfig;
}

/*!
    \qmlproperty NetworkSettingsWireless NetworkService::wirelessConfig
    \readonly
    \brief Holds the wireless configuration for this service.
*/

/*!
    Returns the wireless configuration of the network service.
*/
NetworkSettingsWireless* NetworkSettingsService::wirelessConfig()
{
    Q_D(NetworkSettingsService);
    return &d->m_wifiConfig;
}

/*!
    Sets the placeholder state of the network service to
    \a placeholderState.
*/
void NetworkSettingsService::setPlaceholderState(bool placeholderState)
{
    Q_D(NetworkSettingsService);
    d->setPlaceholderState(placeholderState);
}

/*!
    Returns the placeholder state of the network service.
*/
bool NetworkSettingsService::placeholderState() const
{
    Q_D(const NetworkSettingsService);
    return d->placeholderState();
}

/*!
    \qmlproperty object NetworkService::domains
    \readonly
    \brief The model containing the domains associated with this service.

    The \e domains property can be used as a model for a view
    that lists the domain addresses associated with this service.

    \sa domains.count, domains.append(), domains.remove(), domains.resetChanges()
*/

/*!
    \qmlproperty int NetworkService::domains.count
    \readonly
    \brief Holds the number of domain addresses in the \l domains model.
*/

/*!
   \qmlmethod void NetworkService::domains.append(string address)
   \brief Adds \a address into the \l domains model.
*/

/*!
   \qmlmethod void NetworkService::domains.remove(int index)
   \brief Removes the entry at index \a index from the \l domains model.
*/

/*!
   \qmlmethod void NetworkService::domains.resetChanges()
   \brief Clears unsaved changes from the \l domains model.
*/

/*!
    Returns the model containing the domains associated with this network
    settings service.
*/
QAbstractItemModel* NetworkSettingsService::domains()
{
    Q_D(NetworkSettingsService);
    return &d->m_domainsConfig;
}

/*!
    \qmlproperty object NetworkService::nameservers
    \readonly
    \brief The model containing the domain name servers associated with this
           service.

    The \e nameservers property can be used as a model for a view
    that lists the domain name server (DNS) addresses associated with this
    service.

    \sa nameservers.count, nameservers.append(), nameservers.remove(), nameservers.resetChanges()
*/

/*!
    \qmlproperty int NetworkService::nameservers.count
    \readonly
    \brief Holds the number of domain name server addresses in the
           \l nameservers model.
*/

/*!
   \qmlmethod void NetworkService::nameservers.append(string address)
   \brief Adds \a address into the \l nameservers model.
*/

/*!
   \qmlmethod void NetworkService::nameservers.remove(int index)
   \brief Removes the entry at index \a index from the \l nameservers model.
*/

/*!
   \qmlmethod void NetworkService::nameservers.resetChanges()
   \brief Clears unsaved changes from the \l nameservers model.
*/

/*!
    Returns the model containing the domain name servers associated with this
    network service.
*/
QAbstractItemModel* NetworkSettingsService::nameservers()
{
    Q_D(NetworkSettingsService);
    return &d->m_nameserverConfig;
}

/*!
    \qmlproperty object NetworkService::timeservers
    \readonly
    \brief The model containing the time servers associated with this
           service.

    The \e timeservers property can be used as a model for a view
    that lists the time server addresses associated with this
    service.

    \sa timeservers.count, timeservers.append(), timeservers.remove(), timeservers.resetChanges()
*/

/*!
    \qmlproperty int NetworkService::timeservers.count
    \readonly
    \brief Holds the number of domain name server addresses in the
           \l timeservers model.
*/

/*!
   \qmlmethod void NetworkService::timeservers.append(string address)
   \brief Adds \a address into the \l timeservers model.
*/

/*!
   \qmlmethod void NetworkService::timeservers.remove(int index)
   \brief Removes the entry at index \a index from the \l timeservers model.
*/

/*!
   \qmlmethod void NetworkService::timeservers.resetChanges()
   \brief Clears unsaved changes from the \l timeservers model.
*/

/*!
    Returns the model containing the time servers associated with this
    network service.
*/
QAbstractItemModel* NetworkSettingsService::timeservers()
{
    Q_D(NetworkSettingsService);
    return &d->m_timeserverConfig;
}

/*!
    \qmlmethod void NetworkService::setupIpv4Config()
    \brief Sets up the IPv4 configuration.

    Call this method after changing the IPv4 settings.

    \sa ipv4
*/

/*!
    Sets up the IPv4 configuration.

    Call this method after changing the IPv4 settings.

    \sa ipv4()
*/
void NetworkSettingsService::setupIpv4Config()
{
    Q_D(NetworkSettingsService);
    d->setupIpv4Config();
}

/*!
    \qmlmethod void NetworkService::setupIpv6Config()
    \brief Sets up the IPv6 configuration.

    Call this method after changing the IPv6 settings.

    \sa ipv6
*/

/*!
    Sets up the IPv6 configuration.

    Call this method after changing the IPv6 settings.

    \sa ipv6()
*/
void NetworkSettingsService::setupIpv6Config()
{
    Q_D(NetworkSettingsService);
    d->setupIpv6Config();
}

/*!
    \qmlmethod void NetworkService::setupNameserversConfig()
    \brief Sets up the time server configuration.

    Call this method after modifying the list of time servers.

    \sa timeservers
*/

/*!
    Sets up the time server configuration.

    Call this method after changing the time server settings.

    \sa timeservers
*/
void NetworkSettingsService::setupTimeserversConfig()
{
    Q_D(NetworkSettingsService);
    d->setupTimeserversConfig();
}

/*!
    \qmlmethod void NetworkService::setupNameserversConfig()
    \brief Sets up the domain name server configuration.

    Call this method after modifying the list of domain name servers.

    \sa nameservers
*/

/*!
    Sets up the domain name server configuration.

    Call this method after changing the domain name server settings.

    \sa nameservers
*/
void NetworkSettingsService::setupNameserversConfig()
{
    Q_D(NetworkSettingsService);
    d->setupNameserversConfig();
}

/*!
    \qmlmethod void NetworkService::setupDomainsConfig()
    \brief Sets up the domains configuration.

    Call this method after modifying the list of domain addresses.

    \sa domains
*/

/*!
    Sets up the domain configuration.

    Call this method after modifying the list of domain addresses.

    \sa domains
*/
void NetworkSettingsService::setupDomainsConfig()
{
    Q_D(NetworkSettingsService);
    d->setupDomainsConfig();
}

/*!
    \qmlmethod void NetworkService::setupNetworkSettingsProxy()
    \brief Sets up the network proxy configuration.

    Call this method after modifying the network proxy settings.

    \sa proxy
*/

/*!
    Sets up the network proxy configuration.

    Call this method after modifying the network proxy settings.

    \sa proxy
*/
void NetworkSettingsService::setupNetworkSettingsProxy()
{
    Q_D(NetworkSettingsService);
    d->setupQNetworkSettingsProxy();
}

/*!
    \qmlmethod void NetworkService::connectService()
    \brief Initiates the process of connecting to this network service.

    \sa disconnectService()
*/

/*!
    Initiates the process of connecting to this network service.

    \sa disconnectService()
*/
void NetworkSettingsService::connectService()
{
    emit connectionStateCleared();
    doConnectService();
}

/*!
    Creates a connection to this network service.

    \sa connectService(), disconnectService()
*/
void NetworkSettingsService::doConnectService()
{
    Q_D(NetworkSettingsService);
    d->connectService();
}

/*!
    \qmlmethod void NetworkService::disconnectService()
    \brief Disconnects this service.

    \sa connectService()
*/

/*!
    Disconnects this service.

    \sa connectService()
*/
void NetworkSettingsService::disconnectService()
{
    Q_D(NetworkSettingsService);
    d->disconnectService();
}

/*!
    \qmlmethod void NetworkService::removeService()
    \brief Removes this service from the service cache and clears
    any remembered credentials.
*/

/*!
    Removes this service from the service cache and clears any remembered
    credentials.
*/
void NetworkSettingsService::removeService()
{
    Q_D(NetworkSettingsService);
    d->removeService();
}
