#ifndef NETWORKSETTINGSMANAGERPRIVATE_H
#define NETWORKSETTINGSMANAGERPRIVATE_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QDBusServiceWatcher>
#include "connmancommon.h"
#include "networksettingsmanager.h"
#include "networksettingsinterfacemodel.h"
#include "connman_manager_interface.h"

class QDBusObjectPath;
class QDBusPendingCallWatcher;
class NetworkSettingsService;
class NetworkSettingsServiceModel;
class NetworkSettingsServiceFilter;

class NetworkSettingsManagerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(NetworkSettingsManager)
public:
    explicit NetworkSettingsManagerPrivate(NetworkSettingsManager *parent);
    NetworkSettingsManager *q_ptr;
    void setUserAgent(NetworkSettingsUserAgent *agent);
    NetworkSettingsUserAgent *userAgent() const {return m_agent;}
    NetworkSettingsInterfaceModel* interfaceModel() {return &m_interfaceModel;}
    NetworkSettingsServiceModel* serviceModel() const {return m_serviceModel;}
    NetworkSettingsServiceFilter* serviceFilter() const {return m_serviceFilter;}
    void connectBySsid(const QString &name);
    void clearConnectionState();
    void tryNextConnection();
    void setCurrentWifiConnection(NetworkSettingsService *connection);
    NetworkSettingsService* currentWifiConnection() const;
    void setCurrentWiredConnection(NetworkSettingsService *connection);
    NetworkSettingsService* currentWiredConnection() const;

public slots:
    void getServicesFinished(QDBusPendingCallWatcher *watcher);
    void getTechnologiesFinished(QDBusPendingCallWatcher *watcher);
    void requestInput(const QString& service, const QString& type);
    void onServicesChanged(ConnmanMapStructList changed, const QList<QDBusObjectPath> &removed);
    void serviceReady();
    void onConnmanServiceRegistered(const QString &serviceName);
    void onTechnologyAdded(const QDBusObjectPath &technology, const QVariantMap &properties);
    void onTechnologyRemoved(const QDBusObjectPath &technology);

private:
    bool initialize();
    void handleNewService(const QString &servicePath);

protected:
    NetworkSettingsInterfaceModel m_interfaceModel;
    NetworkSettingsServiceModel *m_serviceModel;
    QMap<QString, NetworkSettingsService*> m_unknownServices;
    QMap<QString, NetworkSettingsService*> m_unnamedServices;
    QMap<QString, NetworkSettingsService*> m_unnamedServicesForSsidConnection;
    NetworkSettingsServiceFilter *m_serviceFilter;
private:
    NetConnmanManagerInterface *m_manager;
    NetworkSettingsUserAgent *m_agent;
    QDBusServiceWatcher *m_serviceWatcher;
    QString m_currentSsid;
    QPointer<NetworkSettingsService> m_currentWifiConnection;
    QPointer<NetworkSettingsService> m_currentWiredConnection;
    bool m_initialized;
};

#endif // NETWORKSETTINGSMANAGERPRIVATE_H
