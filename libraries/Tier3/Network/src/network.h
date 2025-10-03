#ifndef NETWORK_H
#define NETWORK_H

#include <QDefs>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInformation>
#include "qsingleton.h"

class Network : public QObject,
                public QQmlSingleton<Network>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString backend              READ backend                CONSTANT FINAL)
    Q_PROPERTY(QString reachability         READ reachability           NOTIFY reachabilityChanged FINAL)
    Q_PROPERTY(QString transportMedium      READ transportMedium        NOTIFY transportMediumChanged FINAL)
    Q_PROPERTY(QString captivePortal        READ captivePortal          NOTIFY captivePortalChanged FINAL)
    Q_PROPERTY(QString metered              READ metered                NOTIFY meteredChanged FINAL)

    Q_PROPERTY(QString ipv4Address          READ getIPv4Address         NOTIFY informationsChanged FINAL)
    Q_PROPERTY(QString ethernetIPv4Address  READ getEthernetIPv4Address NOTIFY informationsChanged FINAL)
    Q_PROPERTY(QString wifiIPv4Address      READ getWifiIPv4Address     NOTIFY informationsChanged FINAL)
    Q_PROPERTY(QString usbIPv4Address       READ getUsbIPv4Address      NOTIFY informationsChanged FINAL)
    Q_PROPERTY(QString hostname             READ getHostname            NOTIFY informationsChanged FINAL)
    Q_PROPERTY(QString domain               READ getDomain              NOTIFY informationsChanged FINAL)

protected:
    friend QQmlSingleton<Network>;
    explicit Network(QObject *parent = nullptr);

public:
    static QNetworkInformation* netInfo();

    Q_INVOKABLE static QString reachabilityAsString(QNetworkInformation::Reachability reachability);
    Q_INVOKABLE static QString transportMediumAsString(QNetworkInformation::TransportMedium transportMedium);

    static QString backend();
    static QString reachability();
    static QString transportMedium();
    static QString captivePortal();
    static QString metered();

    static QString getIPv4Address();
    static QString getEthernetIPv4Address();
    static QString getWifiIPv4Address();
    static QString getUsbIPv4Address();
    static QString getHostname();
    static QString getDomain();

    Q_INVOKABLE static bool waitForReachability(QNetworkInformation::Reachability reachability, int timeout=30000);

signals:
    void informationsChanged();
    void reachabilityChanged();
    void transportMediumChanged();
    void captivePortalChanged();
    void meteredChanged();
};

#endif // NETWORK_H
