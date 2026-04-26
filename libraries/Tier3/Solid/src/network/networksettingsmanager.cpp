#include "networksettings.h"
#include "networksettingsmanager.h"
#include "networksettingsservice.h"
#include "networksettingsservicemodel.h"
#include "networksettingsinterface.h"
#include "networksettingsinterfacemodel.h"
#include "networksettingsmanager_p.h"
#include "networksettingsuseragent.h"

/*!
    \class NetworkSettingsManager
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsManager class provides an asynchronous API to
    network settings.

    The network manager is designed to be used as a model that contains lists
    of available network interfaces and services.

    A delegate in a view that uses the interface model can access the
    network interface item with the data model role.

    The services list in the model can be controlled with the
    \l NetworkSettingsType::type property, and network service items can be
    retrieved with the \l NetworkSettingsServiceFilter::itemFromRow() method.

    The following code demonstrates how you initialize the network manager:

    \code
    manager = new NetworkSettingsManager(this);
    connect(manager, &NetworkSettingsManager::interfacesChanged,
        this, &MainWindow::interfacesChanged);

    connect(manager, &NetworkSettingsManager::servicesChanged,
        this, &MainWindow::servicesChanged);
    \endcode

    After initializing the network manager, you can iterate WiFi SSIDs as follows:

    \code
    void WifiHandler::servicesChanged()
    {
        QList<NetworkSettingsService*> services = qobject_cast<NetworkSettingsServiceModel*>(manager->services()->sourceModel())->getModel();

        for (const auto &service : services) {
            qDebug()<<service->name();
        }
    }


    void WifiHandler::interfacesChanged()
    {
        QList<NetworkSettingsInterface*> interfaces = manager->interfaces()->getModel();
        for (const auto &interface : interfaces) {
            if (interface->type() == NetworkSettingsType::Type::Wifi) {
                if (interface->powered()) {
                    interface->scanServices();
                } else {
                interface->setPowered(true);
                }
            }
        }
    }
    \endcode

    \sa NetworkSettingsService
*/

/*!
    \property NetworkSettingsManager::services
    \brief Holds the service model.

    \sa NetworkSettingsServiceFilter
*/

/*!
    \property NetworkSettingsManager::interfaces
    \brief Holds the interface model.

    \sa NetworkSettingsInterfaceModel
*/

/*!
    \property NetworkSettingsManager::userAgent
    \brief Holds the user credentials for connecting to a network.

    \sa NetworkSettingsUserAgent
*/

/*!
    \property NetworkSettingsManager::currentWifiConnection
    \brief Holds the current Wifi connection.

    \sa NetworkSettingsService
*/

/*!
    \property NetworkSettingsManager::currentWiredConnection
    \brief Holds the current wired connection.

    \sa NetworkSettingsService
*/

/*!
    \fn NetworkSettingsManager::servicesChanged()

    This signal is emitted when the network service changes.
*/

/*!
    \fn NetworkSettingsManager::interfacesChanged()

    This signal is emitted when the network interface changes.
*/

/*!
    \fn NetworkSettingsManager::currentWifiConnectionChanged()

    This signal is emitted when the current Wifi connection changes.
*/

/*!
    \fn NetworkSettingsManager::currentWiredConnectionChanged()

    This signal is emitted when the current wired connection changes.
*/

/*!
    Creates a network manager with the parent \a parent.
*/
NetworkSettingsManager::NetworkSettingsManager(QObject *parent) :
   QObject(parent)
   ,d_ptr(new NetworkSettingsManagerPrivate(this))
{
}

/*!
    Returns the service model.

    \l NetworkSettingsType::Type
*/
NetworkSettingsServiceFilter *NetworkSettingsManager::services()
{
    Q_D(NetworkSettingsManager);
    return d->serviceFilter();
}

/*!
    Returns the interface model.
*/
NetworkSettingsInterfaceModel *NetworkSettingsManager::interfaces()
{
    Q_D(NetworkSettingsManager);
    return d->interfaceModel();
}

/*!
    Returns the service model \a name of the type \a type.
*/
NetworkSettingsService* NetworkSettingsManager::service(const QString& name, int type)
{
    Q_D(NetworkSettingsManager);

    for (NetworkSettingsService* service: d->serviceModel()->getModel()) {
        if (service->name() == name && service->type() == type) {
            return service;
        }
    }
    return nullptr;
}

/*!
    Creates a connection to the network specified by \a name using the password
    \a passphrase.
*/
void NetworkSettingsManager::connectBySsid(const QString &name, const QString &passphrase)
{
    Q_D(NetworkSettingsManager);
    NetworkSettingsUserAgent* agent = userAgent();
    if (agent)
        agent->setSsidAndPassphrase(name, passphrase);
    d->connectBySsid(name);
}

/*!
    Clears the connection state.
*/
void NetworkSettingsManager::clearConnectionState()
{
    Q_D(NetworkSettingsManager);
    d->clearConnectionState();
    NetworkSettingsUserAgent* agent = userAgent();
    if (agent)
        agent->clearConnectionState();
}

/*!
    Attempts to connect using the next network interface in the model.
*/
void NetworkSettingsManager::tryNextConnection()
{
    Q_D(NetworkSettingsManager);
    d->tryNextConnection();
}

/*!
    Terminates the current connection to the specified \a service.
*/
void NetworkSettingsManager::clearCurrentConnection(NetworkSettingsService* service)
{
    Q_D(NetworkSettingsManager);
    if (service->type() == NetworkSettingsType::Wifi) {
        NetworkSettingsService *currentService = d->currentWifiConnection();
        if (service == currentService) {
            d->setCurrentWifiConnection(nullptr);
            emit currentWifiConnectionChanged();
        }
    } else if (service->type() == NetworkSettingsType::Wired) {
        NetworkSettingsService *currentService = d->currentWiredConnection();
        if (service == currentService) {
            d->setCurrentWiredConnection(nullptr);
            emit currentWiredConnectionChanged();
        }
    }
}

/*!
    Creates a connection to the specified \a service.
*/
void NetworkSettingsManager::setCurrentConnection(NetworkSettingsService* service)
{
    Q_D(NetworkSettingsManager);
    if (service->type() == NetworkSettingsType::Wifi) {
        NetworkSettingsService *currentService = d->currentWifiConnection();
        d->setCurrentWifiConnection(service);
        if (service != currentService) {
            emit currentWifiConnectionChanged();
        }
    } else if (service->type() == NetworkSettingsType::Wired) {
        NetworkSettingsService *currentService = d->currentWiredConnection();
        d->setCurrentWiredConnection(service);
        if (service != currentService) {
            emit currentWiredConnectionChanged();
        }
    }
}

/*!
    Returns the current Wifi connection.
*/
NetworkSettingsService* NetworkSettingsManager::currentWifiConnection()
{
    Q_D(NetworkSettingsManager);
    return d->currentWifiConnection();
}

/*!
    Returns the current wired connection.
*/
NetworkSettingsService* NetworkSettingsManager::currentWiredConnection()
{
    Q_D(NetworkSettingsManager);
    return d->currentWiredConnection();
}

/*!
    Returns the network interface instance \a instance of the type \a type.
*/
NetworkSettingsInterface* NetworkSettingsManager::interface(int type, int instance)
{
    Q_D(NetworkSettingsManager);
    int matchingInstance = 0;

    for (NetworkSettingsInterface* interface: d->m_interfaceModel.getModel()) {
        if (interface->type() == type) {
            if (matchingInstance == instance) {
                return interface;
            }
            matchingInstance++;
        }
    }
    return nullptr;
}

/*!
    Returns the network interface instance at index \a.
*/
NetworkSettingsInterface* NetworkSettingsManager::interface(int index)
{
    Q_D(NetworkSettingsManager);
    if(index<0 || index>=d->m_interfaceModel.rowCount())
        return nullptr;
    return d->m_interfaceModel.getModel().at(index);
}

/*!
    Sets the user credentials for connecting to a network to \a agent.
*/
void NetworkSettingsManager::setUserAgent(NetworkSettingsUserAgent *agent)
{
    Q_D(NetworkSettingsManager);
    d->setUserAgent(agent);
    connect(agent, &NetworkSettingsUserAgent::requestNextConnection,
            this, &NetworkSettingsManager::tryNextConnection);
}

/*!
    Returns the user credentials for connecting to a network.
*/
NetworkSettingsUserAgent* NetworkSettingsManager::userAgent()
{
    Q_D(NetworkSettingsManager);
    return d->userAgent();
}
