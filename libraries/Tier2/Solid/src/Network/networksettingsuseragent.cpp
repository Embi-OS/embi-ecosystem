#include "networksettingsuseragent.h"
#include "networksettingsuseragent_p.h"

/*!
    \class NetworkSettingsUserAgent
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsUserAgent represents the user credentials for
    connecting to a network.

    The NetworkSettingsService::connectService() method is used to connect to
    a Wifi network. The network name is specified by using a service set
    identifier (SSID). If no password is needed, the connection is created
    immediately. NetworkSettingsService::state property can be used to
    monitor the state of the connection.

    If a password is needed, the
    NetworkSettingsUserAgent::showUserCredentialsInput() signal is emitted.
    To create a connection, the password is set by using the
    NetworkSettingsUserAgent::setPassphrase() method.

    \sa NetworkSettingsManager
*/

/*!
    \fn NetworkSettingsUserAgent::showUserCredentialsInput()

    This signal is emitted when user credentials are required for connecting to
    a Wifi network.

    \sa NetworkSettingsService::connectService()
*/

/*!
    \fn NetworkSettingsUserAgent::error()

    This signal is emitted when the connection failed due to invalid user
    credentials.
*/

/*!
    \fn NetworkSettingsUserAgent::ready(bool cancel)

    This signal is emitted when the user has either submitted the password
    for the network, or if \a cancel is \c true, cancelled the password
    query.
*/

/*!
    \fn NetworkSettingsUserAgent::requestNextConnection()

    This signal is emitted when the network manager attempts to connect using
    the next network interface in the model.
*/

/*!
    Creates a user agent with the parent \a parent.
*/
NetworkSettingsUserAgent::NetworkSettingsUserAgent(QObject *parent)
    :QObject(parent)
    ,d_ptr(new NetworkSettingsUserAgentPrivate(this))
{
}

/*!
    Sets the password for connecting to a Wifi network to \a passphrase.

    This method needs to be called in response to receiving the
    \l showUserCredentialsInput() signal.
*/
void NetworkSettingsUserAgent::setPassphrase(const QString &passphrase)
{
    Q_D(NetworkSettingsUserAgent);
    d->setPassphrase(passphrase);
}

/*!
    Cancels the user credentials input request.
*/
void NetworkSettingsUserAgent::cancelInput()
{
    Q_D(NetworkSettingsUserAgent);
    d->cancel();
}

/*!
    Returns the password of the user agent.
*/
QString NetworkSettingsUserAgent::passphrase() const
{
    Q_D(const NetworkSettingsUserAgent);
    return d->passphrase();
}

/*!
    Sets the name of the network to \a ssid and the password of the user agent
    to \a passphrase.
*/
void NetworkSettingsUserAgent::setSsidAndPassphrase(const QString &ssid, const QString &passphrase)
{
    Q_D(NetworkSettingsUserAgent);
    d->setSsidAndPassphrase(ssid, passphrase);
}

/*!
    Clears the connection state.
*/
void NetworkSettingsUserAgent::clearConnectionState()
{
    Q_D(NetworkSettingsUserAgent);
    d->clearConnectionState();
}
