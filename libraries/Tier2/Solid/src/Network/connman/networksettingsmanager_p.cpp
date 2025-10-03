#include "networksettingsservice.h"
#include "networksettingsmanager_p.h"
#include "networksettingsinterface_p.h"
#include "networksettingsservicemodel.h"
#include "networksettingsuseragent.h"
#include "moc_connman_manager_interface.cpp"

#include "solid_log.h"

const QString ConnManServiceName(QStringLiteral("net.connman"));

NetworkSettingsManagerPrivate::NetworkSettingsManagerPrivate(NetworkSettingsManager *parent)
    :QObject(parent)
    ,q_ptr(parent)
    , m_interfaceModel(nullptr)
    , m_serviceModel(nullptr)
    , m_serviceFilter(nullptr)
    , m_manager(nullptr)
    , m_agent(nullptr)
    , m_serviceWatcher(nullptr)
    , m_currentWifiConnection(nullptr)
    , m_currentWiredConnection(nullptr)
    , m_initialized(false)
{
    qRegisterMetaType<ConnmanMapStruct>("ConnmanMapStruct");
    qRegisterMetaType<ConnmanMapStructList>("ConnmanMapStructList");
    qDBusRegisterMetaType<ConnmanMapStruct>();
    qDBusRegisterMetaType<ConnmanMapStructList>();

    NetworkSettingsUserAgent* userAgent = new NetworkSettingsUserAgent(this);
    this->setUserAgent(userAgent);

    m_serviceModel = new NetworkSettingsServiceModel(this);
    m_serviceFilter = new NetworkSettingsServiceFilter(this);
    m_serviceFilter->setSourceModel(m_serviceModel);

    QDBusConnectionInterface* bus = QDBusConnection::systemBus().interface();
    if (bus->isServiceRegistered(ConnManServiceName)) {
        if (!initialize()) {
            SOLIDLOG_WARNING()<<"Failed to initialize connman connection";
        }
    } else {
        m_serviceWatcher = new QDBusServiceWatcher(this);
        m_serviceWatcher->setConnection(QDBusConnection::systemBus());
        m_serviceWatcher->setWatchedServices(QStringList({ConnManServiceName}));
        connect(m_serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, &NetworkSettingsManagerPrivate::onConnmanServiceRegistered);
    }
}

bool NetworkSettingsManagerPrivate::initialize()
{
    if (m_initialized && m_manager)
        return m_initialized;

    m_manager = new NetConnmanManagerInterface(ConnManServiceName, QStringLiteral("/"),
            QDBusConnection::systemBus(), this);

    if (m_manager->isValid()) {
        //List technologies
        QDBusPendingReply<ConnmanMapStructList> reply = m_manager->GetTechnologies();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &NetworkSettingsManagerPrivate::getTechnologiesFinished);

        reply = m_manager->GetServices();
        watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, &QDBusPendingCallWatcher::finished,
               this, &NetworkSettingsManagerPrivate::getServicesFinished);

        connect(m_manager, &NetConnmanManagerInterface::ServicesChanged, this, &NetworkSettingsManagerPrivate::onServicesChanged);
        connect(m_manager, &NetConnmanManagerInterface::TechnologyAdded, this, &NetworkSettingsManagerPrivate::onTechnologyAdded);
        connect(m_manager, &NetConnmanManagerInterface::TechnologyRemoved, this, &NetworkSettingsManagerPrivate::onTechnologyRemoved);

        m_manager->RegisterAgent(QDBusObjectPath(Q_PropertyAgentPath));
        m_initialized = true;
    } else {
        delete m_manager;
        m_manager = nullptr;
        m_initialized = false;
    }
    return m_initialized;
}

void NetworkSettingsManagerPrivate::requestInput(const QString& service, const QString& type)
{
    Q_UNUSED(service)
    Q_UNUSED(type)
    emit m_agent->showUserCredentialsInput();
}

void NetworkSettingsManagerPrivate::connectBySsid(const QString &name)
{
    m_unnamedServicesForSsidConnection = m_unnamedServices;
    tryNextConnection();
    m_currentSsid = name;
}

void NetworkSettingsManagerPrivate::clearConnectionState()
{
    m_unnamedServicesForSsidConnection.clear();
    m_currentSsid.clear();
}

void NetworkSettingsManagerPrivate::tryNextConnection()
{
    Q_Q(NetworkSettingsManager);
    NetworkSettingsService* service = nullptr;
    if (!m_currentSsid.isEmpty()) {
        service = m_serviceModel->getByName(m_currentSsid);
        m_currentSsid.clear();
    }
    if (!service) {
        if (!m_unnamedServicesForSsidConnection.isEmpty()) {
            service = m_unnamedServicesForSsidConnection.take(m_unnamedServicesForSsidConnection.firstKey());
        } else {
            q->clearConnectionState();
        }
    }
    if (service) {
        service->doConnectService();
    }
}

void NetworkSettingsManagerPrivate::setCurrentWifiConnection(NetworkSettingsService *connection)
{
    m_currentWifiConnection = connection;
}

NetworkSettingsService *NetworkSettingsManagerPrivate::currentWifiConnection() const
{
    return m_currentWifiConnection.data();
}

void NetworkSettingsManagerPrivate::setCurrentWiredConnection(NetworkSettingsService *connection)
{
    m_currentWiredConnection = connection;
}

NetworkSettingsService *NetworkSettingsManagerPrivate::currentWiredConnection() const
{
    return m_currentWiredConnection.data();
}

void NetworkSettingsManagerPrivate::onConnmanServiceRegistered(const QString &serviceName)
{
    if (serviceName == ConnManServiceName) {
        if (!initialize()) {
            SOLIDLOG_WARNING()<<"Failed to initialize connman connection";
        }
    }
}

void NetworkSettingsManagerPrivate::onTechnologyAdded(const QDBusObjectPath &technology, const QVariantMap &properties)
{
    Q_Q(NetworkSettingsManager);

    for (NetworkSettingsInterface* item: m_interfaceModel.getModel()) {
        ConnmanSettingsInterface* interface = qobject_cast<ConnmanSettingsInterface*>(item);
        if (interface->path() == technology.path()) {
            return; // we already know the interface/technology
        }
    }

    ConnmanSettingsInterface *interface = new ConnmanSettingsInterface(technology.path(), properties, this);
    interface->scanServices();

    if (interface->type() == NetworkSettingsType::Wired) {
        m_interfaceModel.insert(0, interface);
    }
    else if (interface->type() == NetworkSettingsType::Wifi) {
        m_interfaceModel.append(interface);
    }

    emit q->interfacesChanged();
}

void NetworkSettingsManagerPrivate::onTechnologyRemoved(const QDBusObjectPath &technology)
{
    Q_Q(NetworkSettingsManager);

    for (NetworkSettingsInterface* item: m_interfaceModel.getModel()) {
        ConnmanSettingsInterface* tech = qobject_cast<ConnmanSettingsInterface*>(item);
        if (tech->path() == technology.path()) {
            m_interfaceModel.removeInterface(tech->name());
            emit q->interfacesChanged();
        }
    }
}

void NetworkSettingsManagerPrivate::getServicesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<ConnmanMapStructList> reply = *watcher;
    watcher->deleteLater();

    if (reply.isError())
        return;

    for (const ConnmanMapStruct &object: reply.value()) {
        const QString servicePath = object.objectPath.path();
        handleNewService(servicePath);
    }
}

void NetworkSettingsManagerPrivate::getTechnologiesFinished(QDBusPendingCallWatcher *watcher)
{
    Q_Q(NetworkSettingsManager);

    QDBusPendingReply<ConnmanMapStructList> reply = *watcher;
    watcher->deleteLater();
    if (reply.isError())
        return;

    for (const ConnmanMapStruct &object: reply.value()) {
        ConnmanSettingsInterface *item = new ConnmanSettingsInterface(object.objectPath.path(), object.propertyMap, this);
        item->scanServices();

        if (item->type() == NetworkSettingsType::Wired) {
            m_interfaceModel.insert(0, item);
        }
        else if (item->type() == NetworkSettingsType::Wifi) {
            m_interfaceModel.append(item);
        }
        emit q->interfacesChanged();
    }
}

void NetworkSettingsManagerPrivate::onServicesChanged(ConnmanMapStructList changed, const QList<QDBusObjectPath> &removed)
{
    Q_Q(NetworkSettingsManager);
    for (const QDBusObjectPath &dpath: removed) {
        QString path = dpath.path();

        if (m_serviceModel->removeService(path))
            emit q->servicesChanged();

        if (auto service = m_unnamedServices.take(path))
            if (!service->placeholderState()) {
                disconnect(service, &NetworkSettingsService::nameChanged,
                           this, &NetworkSettingsManagerPrivate::serviceReady);
                disconnect(service, &NetworkSettingsService::typeChanged,
                           this, &NetworkSettingsManagerPrivate::serviceReady);
                service->deleteLater();
            }
        if (auto service = m_unknownServices.take(path))
            if (!service->placeholderState()) {
                disconnect(service, &NetworkSettingsService::nameChanged,
                           this, &NetworkSettingsManagerPrivate::serviceReady);
                disconnect(service, &NetworkSettingsService::typeChanged,
                           this, &NetworkSettingsManagerPrivate::serviceReady);
                service->deleteLater();
            }
        m_unnamedServicesForSsidConnection.remove(path); // do not delete here
    }

    QStringList newServices;
    for (const ConnmanMapStruct &map: changed) {
        QString path = map.objectPath.path();

        if (m_unknownServices.contains(path) || m_unnamedServices.contains(path))
            continue;

        bool found = false;
        for (NetworkSettingsService* service: m_serviceModel->getModel()) {
            if (service->id() == path && service->placeholderState() == false) {
                found = true;
                break;
            }
        }
        if (!found)
            newServices.append(map.objectPath.path());
    }

    for (const QString &newService: newServices) {
        handleNewService(newService);
    }

}

void NetworkSettingsManagerPrivate::handleNewService(const QString &servicePath)
{
    Q_Q(NetworkSettingsManager);

    NetworkSettingsService *service = new NetworkSettingsService(servicePath, this);

    connect(service, &NetworkSettingsService::connectionStateCleared,
            q, &NetworkSettingsManager::clearConnectionState);

    connect(service, &NetworkSettingsService::serviceConnected,
            q, &NetworkSettingsManager::setCurrentConnection);
    connect(service, &NetworkSettingsService::serviceDisconnected,
            q, &NetworkSettingsManager::clearCurrentConnection);

    if (service->name().length() > 0 && service->type() != NetworkSettingsType::Unknown) {
        m_serviceModel->append(service);
        emit q->servicesChanged();
        if (service->type() == NetworkSettingsType::Wired) {
            m_serviceFilter->setWiredNetworksAvailable(true);
        }
    }
    else {
        bool isUnnamedWifi = false;

        //Service name or type not set, wait for update
        if (service->name().isEmpty()) {
            connect(service, &NetworkSettingsService::nameChanged, this, &NetworkSettingsManagerPrivate::serviceReady);
            isUnnamedWifi = (service->type() == NetworkSettingsType::Wifi);
        }
        if (service->type() == NetworkSettingsType::Unknown)
            connect(service, &NetworkSettingsService::typeChanged, this, &NetworkSettingsManagerPrivate::serviceReady);

        if (isUnnamedWifi)
            m_unnamedServices.insert(service->id(), service);
        else
            m_unknownServices.insert(service->id(), service);
    }
}

void NetworkSettingsManagerPrivate::setUserAgent(NetworkSettingsUserAgent *agent)
{
    m_agent = agent;
}

void NetworkSettingsManagerPrivate::serviceReady()
{
    Q_Q(NetworkSettingsManager);

    NetworkSettingsService* service = qobject_cast<NetworkSettingsService*>(sender());

    // the type changed from Unknown to Wifi
    if ((service->type() == NetworkSettingsType::Wifi)
            && m_unknownServices.contains(service->id())) {
        m_unknownServices.remove(service->id());
        m_unnamedServices.insert(service->id(), service);
    }

    // we have a name and a length now
    if (service->name().length() > 0 && service->type() != NetworkSettingsType::Unknown) {
        service->disconnect(this);
        m_unnamedServices.remove(service->id());
        m_serviceModel->append(service);
        emit q->servicesChanged();
        if (service->type() == NetworkSettingsType::Wired) {
            m_serviceFilter->setWiredNetworksAvailable(true);
        }

        //Update the interface state accordingly
        for (NetworkSettingsInterface* item: m_interfaceModel.getModel()) {
            ConnmanSettingsInterface* technology = qobject_cast<ConnmanSettingsInterface*>(item);
            if (technology->name() == service->name() && technology->type() == service->type()) {
                technology->setState(technology->state());
            }
        }
    }
}
