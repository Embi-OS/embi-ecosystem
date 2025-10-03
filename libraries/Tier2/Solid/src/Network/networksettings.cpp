#include "networksettings.h"

/*!
    \module QtNetworkSettings
    \qtvariable networksettings
    \ingroup qtdevice-utilities-cpp-modules
    \ingroup modules
    \title Qt Network Settings C++ Classes
    \brief Provides functionality for controlling network settings.

    To use classes from this module, add this directive into the C++ files:

    \code
    #include <QtNetworkSettings>
    \endcode

    To link against the corresponding C++ libraries when using CMake to build you application,
    add the following to your CMakeLists.txt file:

    \code
    find_package(Qt6 COMPONENTS NetworkSettings REQUIRED)
    target_link_libraries(mytarget Qt6::NetworkSettings)
    \endcode

    Or if you are using qmake to build your application, add the following line to your qmake .pro project file:

    \code
    QT += networksettings
    \endcode
*/

/*!
    \class NetworkSettingsState
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsState class represents the network interface
    state.

    \sa NetworkSettingsManager
*/

/*!
    \enum NetworkSettingsState::State

    This enum type holds the state of the network interface.

    \value  Idle
    \value  Failure
            Failed to connect.
    \value  Association
            Authentication in progress.
    \value  Configuration
            Configuration in progress.
    \value  Ready
            Connected to a network.
    \value  Disconnect
            Disconnected from a network.
    \value  Online
            Acquired an IP address.
    \value  Undefined
            Undefined state.
*/

/*!
    \property NetworkSettingsState::state
    \brief The state of the network interface.

    \sa NetworkSettingsState::State
*/

/*!
    \fn NetworkSettingsState::stateChanged()
    This signal is emitted when the state of the network interface changes.

    \sa NetworkSettingsState::State
*/

/*!
    Creates a new network interface state object with the state \a state and the
    parent \a parent.
*/
NetworkSettingsState::NetworkSettingsState(State state, QObject *parent)
    : QObject(parent)
    , m_state(state)
{
}

/*!
    Creates a new network interface state object with the parent \a parent.
*/
NetworkSettingsState::NetworkSettingsState(QObject *parent)
    : QObject(parent)
    , m_state(Undefined)
{
}

/*!
    Returns the network interface state.
*/
NetworkSettingsState::State NetworkSettingsState::state() const {
    return m_state;
}

/*!
    Sets the network interface state to \a state.
*/
void NetworkSettingsState::setState(const State state) {
    m_state = state;
    emit stateChanged();
}

/*!
    \class NetworkSettingsType
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsType class represents the network interface
    type.

    \sa NetworkSettingsManager
*/

/*!
    \enum NetworkSettingsType::Type

    This enum type holds the type of the network interface.

    \value  Wired
            Wired network
    \value  Wifi
            Wifi network
    \value  Bluetooth
            Bluetooth network
    \value  Unknown
            Unknown network type
*/

/*!
    \property NetworkSettingsType::type
    \brief The type of the network interface.

    \sa NetworkSettingsType::Type
*/

/*!
    \fn NetworkSettingsType::typeChanged()
    This signal is emitted when the type of the network interface changes.

    \sa NetworkSettingsType::Type
*/

/*!
    Creates a new network interface type object with the type \a type and
    parent \a parent.
*/
NetworkSettingsType::NetworkSettingsType(Type type, QObject *parent)
    : QObject(parent)
    , m_type(type)
{
    m_type = type;
}

/*!
    Creates a new network interface type object with the parent \a parent.
*/
NetworkSettingsType::NetworkSettingsType(QObject *parent)
    : QObject(parent)
{
    m_type = Unknown;
}

/*!
    Returns the network interface type.
*/
NetworkSettingsType::Type NetworkSettingsType::type() const {
    return m_type;
}

/*!
    Sets the network interface type to \a type.
*/
void NetworkSettingsType::setType(const Type type) {
    m_type = type;
    emit typeChanged();
}

/*!
    \class NetworkSettingsIPv4
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsIPv4 class encapsulates IPv4 network
    configuration.
*/

/*!
    \enum NetworkSettingsIPv4::Method

    This enum type holds the method used for IPv4 configuration.

    \value  Dhcp
            The DHCP protocol is used for the network configuration.
    \value  Manual
            The network is configured manually.
    \value  Off
            The network is not configured.
*/

/*!
    \property NetworkSettingsIPv4::address
    \brief Holds the IPv4 address.
*/

/*!
    \property NetworkSettingsIPv4::gateway
    \brief Holds the IPv4 gateway address.
*/

/*!
    \property NetworkSettingsIPv4::method
    \brief Holds the method of IPv4 configuration.

    \sa NetworkSettingsIPv4::Method
*/

/*!
    \property NetworkSettingsIPv4::mask
    \brief Holds the IPv4 network mask.
*/

/*!
    Creates a new IPv4 network configuration with the parent \a parent.
*/
NetworkSettingsIPv4::NetworkSettingsIPv4(QObject *parent)
    : QObject(parent)
{
}

/*!
    \class NetworkSettingsIPv6
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsIPv6 class encapsulates IPv6 network
    configuration.
*/

/*!
    \enum NetworkSettingsIPv6::Method

    This enum type holds the method used for IPv6 configuration.

    \value  Auto
            The network is configured automatically.
    \value  Manual
            The network is configured manually.
    \value  Off
            The network is not configured.
*/

/*!
    \enum NetworkSettingsIPv6::Privacy
    \brief Holds the method of applying the privacy extensions in IPv6.

    \value  Disabled
            Disables privacy extensions in IPv6
    \value  Enabled
            Enables \l {https://tools.ietf.org/html/rfc4941}
            {Privacy Extensions for Stateless Address Autoconfiguration in IPv6}.
    \value  Preferred
            Enables privacy extensions and gives preference to the use of
            temporary addresses, even when a public address is available
*/

/*!
    \property NetworkSettingsIPv6::address
    \brief Holds the IPv6 address.
*/

/*!
    \property NetworkSettingsIPv6::gateway
    \brief Holds the IPv6 gateway address.
*/

/*!
    \property NetworkSettingsIPv6::method
    \brief Holds the method of IPv6 configuration.

    \sa NetworkSettingsIPv6::Method
*/

/*!
    \property NetworkSettingsIPv6::privacy
    \brief Holds the method of applying privacy extensions for IPv6.

    \sa NetworkSettingsIPv6::Privacy
*/

/*!
    \property NetworkSettingsIPv6::prefixLength
    \brief Holds the IPv6 network prefix length in bits.
*/


/*!
    Creates a new IPv6 network configuration with the parent \a parent.
*/
NetworkSettingsIPv6::NetworkSettingsIPv6(QObject *parent)
    : QObject(parent)
{
}


/*!
    \class NetworkSettingsProxy
    \inmodule QtNetworkSettings
    \brief The NetworkSettingsProxy class encapsulates network proxy
    configuration.
*/

/*!
    \enum NetworkSettingsProxy::Method
    \brief Holds the network proxy configuration method.

    \value  Direct
            Direct network connection, no proxy in use
    \value  Auto
            Automatic proxy configuration
    \value  Manual
            Manual proxy configuration

    \sa url
*/

/*!
    \property NetworkSettingsProxy::method
    \brief Holds the network proxy configuration method.

    \sa NetworkSettingsProxy::Method
*/

/*!
    \property NetworkSettingsProxy::url
    \brief Holds the proxy URL.

    For manual proxy configuration, this property holds the proxy server
    address. For automatic configuration, it holds the proxy auto-config URL.

    \sa NetworkSettingsProxy::Method
*/

/*!
    \property NetworkSettingsProxy::excludes
    \readonly
    \brief The model containing the proxy exclusion list.

    The addresses in the proxy exclusion list are accessed directly,
    instead of forwarding the requests to a proxy.

    This property can be used as a model for a view
    that lists the proxy exclusion addresses.

    \sa NetworkSettingsAddressModel
*/

/*!
    \property NetworkSettingsProxy::servers
    \readonly
    \brief The model containing the proxy exclusion list.

    This property holds the list of proxy server names.
*/

/*!
    Creates a new proxy configuration with the parent \a parent.
*/
NetworkSettingsProxy::NetworkSettingsProxy(QObject *parent)
    : QObject(parent)
{

}

QUrl NetworkSettingsProxy::url() const {
    return m_url;
}

void NetworkSettingsProxy::setUrl(const QUrl& url) {
    m_url = url;
    emit urlChanged();
}

QAbstractItemModel* NetworkSettingsProxy::servers(void) {
    return &m_servers;
}

/*!
    Sets the proxy server list.
    \a servers lists the server names.

    The NetworkSettingsProxy::serversChanged signal is emitted when the server list
    is set.
*/
void NetworkSettingsProxy::setServers(const QStringList& servers) {
    m_servers.setStringList(servers);
    emit serversChanged();
}

QStringList NetworkSettingsProxy::servers() const {
    return m_servers.stringList();
}

QAbstractItemModel* NetworkSettingsProxy::excludes(void) {
    return &m_excludes;
}

QStringList NetworkSettingsProxy::excludes() const {
    return m_excludes.stringList();
}

/*!
    Sets the proxy exclusion list.
    \a excludes lists the addresses on the exclustion list.

    The NetworkSettingsProxy::excludesChanged signal is emitted when the proxy
    exclusion list is set.
*/
void NetworkSettingsProxy::setExcludes(const QStringList& excludes) {
    m_excludes.setStringList(excludes);
    emit excludesChanged();
}

NetworkSettingsProxy::Method NetworkSettingsProxy::method(void) const {
    return m_method;
}

void NetworkSettingsProxy::setMethod(const Method& method) {
    m_method = method;
    emit methodChanged();
}

/*!
    \class NetworkSettingsWireless
    \inmodule QtNetworkSettings
    \brief The NetworkSettingsWireless class encapsulates the configuration
    for a Wifi network service.
*/

/*!
    \enum NetworkSettingsWireless::Security
    \brief This enum type holds the wireless security protocol used to
    protect the connection.

    \value  None
            Not protected
    \value  WEP
            Wired Equivalent Privacy (WEP)
    \value  WPA
            Wi-Fi Protected Access (WPA)
    \value  WPA2
            Wi-Fi Protected Access, version 2 (WPA2)
*/

/*!
    \property NetworkSettingsWireless::signalStrength
    \brief Holds the Wifi signal strength, in the range from 0 to 100.
*/

/*!
    \property NetworkSettingsWireless::hidden
    \readonly
    \brief Holds whether the wireless SSID is hidden.
*/

/*!
    \property NetworkSettingsWireless::isOutOfRange
    \brief Holds whether the Wifi access point is out of range.
*/

/*!
    \fn void NetworkSettingsWireless::passwordChanged()

    This signal is sent when the password has changed.
*/

/*!
    Creates a new Wifi network configuration with the parent \a parent.
*/
NetworkSettingsWireless::NetworkSettingsWireless(QObject* parent)
  : QObject(parent) {
}

/*!
    Returns whether the Wifi network supports the wireless security
    protocol specified in \a security.

    \sa NetworkSettingsWireless::Security
*/
bool NetworkSettingsWireless::supportsSecurity(Security security) {
    return m_supportedSecurites.testFlag(security);
}

bool NetworkSettingsWireless::hidden() const {
    return m_hidden;
}

/*!
    Sets \a hidden status.

    The NetworkSettingsWireless::hiddenChanged signal is emitted when the
    hidden status is changed.
*/
void NetworkSettingsWireless::setHidden(const bool hidden) {
    m_hidden = hidden;
    emit hiddenChanged();
}

int NetworkSettingsWireless::signalStrength() const {
    return m_signalStrength;
}

void NetworkSettingsWireless::setSignalStrength(const int signalStrength) {
    m_signalStrength = signalStrength;
    emit signalStrengthChanged();
}

/*!
    Sets \a securities flags.
*/
void NetworkSettingsWireless::setSecurity(const Securities securities) {
    m_supportedSecurites = securities;
}

void NetworkSettingsWireless::setOutOfRange(const bool aOutOfRange) {
    m_isOutOfRange = aOutOfRange;
    emit outOfRangeChanged();
}

bool NetworkSettingsWireless::outOfRange() const {
    return m_isOutOfRange;
}

/*!
    \class NetworkSettingsEthernet
    \inmodule QtNetworkSettings
    \brief The NetworkSettingsEthernet class encapsulates the configuration
    for a Ethernet network service.
*/

/*!
    Creates a new Wifi network configuration with the parent \a parent.
*/
NetworkSettingsEthernet::NetworkSettingsEthernet(QObject* parent)
    : QObject(parent) {
}
