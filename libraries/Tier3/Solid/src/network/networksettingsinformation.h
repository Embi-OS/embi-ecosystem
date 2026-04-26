#ifndef NETWORKSETTINGSINFORMATION_H
#define NETWORKSETTINGSINFORMATION_H

#include <QDefs>
#include <QNetworkInformation>
#include "qsingleton.h"

class NetworkSettingsInformation : public QObject,
                                   public QQmlSingleton<NetworkSettingsInformation>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isInitialized READ isInitialized CONSTANT FINAL)
    Q_PROPERTY(QNetworkInformation::Reachability reachability READ reachability NOTIFY reachabilityChanged FINAL)
    Q_PROPERTY(QNetworkInformation::TransportMedium transportMedium READ transportMedium NOTIFY transportMediumChanged FINAL)

protected:
    friend QQmlSingleton<NetworkSettingsInformation>;
    explicit NetworkSettingsInformation(QObject *parent = nullptr);

public:
    static NetworkSettingsInformation* netInfo();

    bool isInitialized() const;
    bool supports(QNetworkInformation::Features features) const;
    QNetworkInformation::Features supportedFeatures() const;

    QNetworkInformation::Reachability reachability() const;
    QNetworkInformation::TransportMedium transportMedium() const;

    Q_INVOKABLE static bool waitForReachability(QNetworkInformation::Reachability reachability, int timeout=30000);

Q_SIGNALS:
    void reachabilityChanged(QNetworkInformation::Reachability newReachability);
    void transportMediumChanged(QNetworkInformation::TransportMedium current);

private:
    void evaluateInterfaces();
};

#endif // NETWORKSETTINGSINFORMATION_H
