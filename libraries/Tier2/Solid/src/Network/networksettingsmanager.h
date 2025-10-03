#ifndef NETWORKSETTINGSMANAGER_H
#define NETWORKSETTINGSMANAGER_H

#include <QDefs>
#include "qsingleton.h"

#include "networksettingsservice.h"
#include "networksettingsinterface.h"
#include "networksettingsuseragent.h"
#include "networksettingsservicemodel.h"
#include "networksettingsinterfacemodel.h"

class NetworkSettingsManagerPrivate;
class NetworkSettingsManager : public QObject,
                               public QQmlSingleton<NetworkSettingsManager>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(NetworkSettingsServiceFilter* services READ services NOTIFY servicesChanged FINAL)
    Q_PROPERTY(NetworkSettingsInterfaceModel* interfaces READ interfaces NOTIFY interfacesChanged FINAL)
    Q_PROPERTY(NetworkSettingsUserAgent* userAgent READ userAgent CONSTANT FINAL)
    Q_PROPERTY(NetworkSettingsService* currentWifiConnection READ currentWifiConnection NOTIFY currentWifiConnectionChanged FINAL)
    Q_PROPERTY(NetworkSettingsService* currentWiredConnection READ currentWiredConnection NOTIFY currentWiredConnectionChanged FINAL)
    Q_MOC_INCLUDE("networksettingsservicemodel.h")
    Q_MOC_INCLUDE("networksettingsuseragent.h")

protected:
    friend QQmlSingleton<NetworkSettingsManager>;
    explicit NetworkSettingsManager(QObject *parent = nullptr);

public:
    NetworkSettingsServiceFilter* services();
    NetworkSettingsInterfaceModel* interfaces();
    void setUserAgent(NetworkSettingsUserAgent *agent);
    NetworkSettingsUserAgent* userAgent();

    Q_INVOKABLE NetworkSettingsService* service(const QString& name, int type);
    Q_INVOKABLE void connectBySsid(const QString& name, const QString &passphrase);
    void clearConnectionState();
    void tryNextConnection();
    void clearCurrentConnection(NetworkSettingsService* service);
    void setCurrentConnection(NetworkSettingsService* service);
    NetworkSettingsService* currentWifiConnection();
    NetworkSettingsService* currentWiredConnection();
    Q_INVOKABLE NetworkSettingsInterface* interface(int type, int instance);
    Q_INVOKABLE NetworkSettingsInterface* interface(int index);

Q_SIGNALS:
    void servicesChanged();
    void interfacesChanged();
    void currentWifiConnectionChanged();
    void currentWiredConnectionChanged();

protected:
    NetworkSettingsManagerPrivate *d_ptr;

private:
    Q_DISABLE_COPY(NetworkSettingsManager)
    Q_DECLARE_PRIVATE(NetworkSettingsManager)
};

#endif // NETWORKSETTINGSMANAGER_H
