#include "networksettingsinterface.h"
#include "networksettingsinterface_p.h"

/*!
    \class NetworkSettingsInterface
    \inmodule QtNetworkSettings
    \brief Represents a network interface.

    The NetworkSettingsInterface class represents a network interface attached
    to the host.

    Instances of this class cannot be created directly. Instead, they can be
    retrieved via NetworkSettingsManager::interfaces().
*/

/*!
    \property NetworkSettingsInterface::state
    \readonly
    \brief Holds the state of the network interface.

    \sa NetworkSettingsState::State
*/

/*!
    \property NetworkSettingsInterface::name
    \readonly
    \brief Holds the name of the network interface.
*/

/*!
    \property NetworkSettingsInterface::powered
    \brief Holds whether the network interface is powered on of off.
*/

/*!
    \property NetworkSettingsInterface::type
    \readonly
    \brief Holds the type of the network interface.

    \sa NetworkSettingsType::Type
*/

/*!
    \qmltype NetworkInterface
    \inqmlmodule QtDeviceUtilities.NetworkSettings
    \brief Represents a network interface.

    The NetworkInterface QML type represents a network interface attached
    to the host.

    Instances of NetworkInterface cannot be created directly; instead, they can
    be retrieved via NetworkSettingsManager.

    \sa {NetworkSettingsManager::interfaces}{NetworkSettingsManager.interfaces}
*/


/*!
    Creates a new network interface with the parent \a parent.
*/
NetworkSettingsInterface::NetworkSettingsInterface(QObject *parent) :
    QObject(parent)
    ,d_ptr(new NetworkSettingsInterfacePrivate(this))
{

}

/*!
    \qmlproperty string NetworkInterface::name
    \readonly
    \brief Holds the name of the network interface.
*/

/*!
    Returns the name of the network interface.
*/
QString NetworkSettingsInterface::name() const
{
    Q_D(const NetworkSettingsInterface);
    return d->name();
}

/*!
    \qmlproperty enumeration NetworkInterface::state
    \readonly
    \brief Holds the state of the network interface.

    Possible values:

    \value NetworkSettingsState.Idle
           Idle

    \value NetworkSettingsState.Failure
           Failed to connect

    \value NetworkSettingsState.Association
           Authentication in progress

    \value NetworkSettingsState.Configuration
           Configuration in progress

    \value NetworkSettingsState.Ready
           Connected to a network

    \value NetworkSettingsState.Disconnect
           Disconnected from a network

    \value NetworkSettingsState.Online
           Online (acquired an IP address)

    \value NetworkSettingsState.Undefined
           Undefined state.
*/

/*!
    Returns the state of the network interface.
*/
NetworkSettingsState::State NetworkSettingsInterface::state()
{
    Q_D(NetworkSettingsInterface);
    return d->state();
}

/*!
    \qmlproperty enumeration NetworkInterface::type
    \readonly
    \brief Holds the type of the network interface.

    Possible values:

    \value NetworkSettingsType.Wired     Wired network
    \value NetworkSettingsType.Wifi      Wifi network
    \value NetworkSettingsType.Bluetooth Bluetooth network
    \value NetworkSettingsType.Unknown   Unknown network type
*/

/*!
    Returns the type of the network interface.
*/
NetworkSettingsType::Type NetworkSettingsInterface::type()
{
    Q_D(NetworkSettingsInterface);
    return d->type();
}

/*!
    \qmlproperty bool NetworkInterface::powered
    \brief Holds whether the network interface is powered on or off.
*/

/*!
    Returns whether the network interface is powered on or off.
*/
bool NetworkSettingsInterface::powered() const
{
    Q_D(const NetworkSettingsInterface);
    return d->powered();
}

/*!
    Sets the powered state in the network interface to \a powered.
*/
void NetworkSettingsInterface::setPowered(const bool powered)
{
    Q_D(NetworkSettingsInterface);
    d->setPowered(powered);
}

/*!
    \qmlmethod void NetworkInterface::scanServices()
    \brief Initiates a scan for network interface services.
*/

/*!
    Initiates a scan for network interface services.
*/
void NetworkSettingsInterface::scanServices()
{
    Q_D(NetworkSettingsInterface);
    d->scan();
}
