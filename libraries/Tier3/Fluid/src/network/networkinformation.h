#ifndef NETWORKINFORMATION_H
#define NETWORKINFORMATION_H

#include <QDefs>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInformation>
#include "qsingleton.h"

class NetworkInformation : public QObject,
                           public QQmlSingleton<NetworkInformation>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString backend              READ backend                CONSTANT FINAL)
    Q_PROPERTY(QString reachability         READ reachability           NOTIFY reachabilityChanged FINAL)
    Q_PROPERTY(QString transportMedium      READ transportMedium        NOTIFY transportMediumChanged FINAL)
    Q_PROPERTY(QString captivePortal        READ captivePortal          NOTIFY captivePortalChanged FINAL)
    Q_PROPERTY(QString metered              READ metered                NOTIFY meteredChanged FINAL)

    Q_PROPERTY(QString ipv4Address          READ ipv4Address            NOTIFY ipv4AddressChanged FINAL)
    Q_PROPERTY(QString ethernetIPv4Address  READ ethernetIPv4Address    NOTIFY ipv4AddressChanged FINAL)
    Q_PROPERTY(QString wifiIPv4Address      READ wifiIPv4Address        NOTIFY ipv4AddressChanged FINAL)
    Q_PROPERTY(QString usbIPv4Address       READ usbIPv4Address         NOTIFY ipv4AddressChanged FINAL)
    Q_PROPERTY(QString localHostName        READ localHostName CONSTANT FINAL)
    Q_PROPERTY(QString localDomainName      READ localDomainName CONSTANT FINAL)

protected:
    friend QQmlSingleton<NetworkInformation>;
    explicit NetworkInformation(QObject *parent = nullptr);

public:
    static QNetworkInformation* netInfo();

    Q_INVOKABLE static QString reachabilityAsString(QNetworkInformation::Reachability reachability);
    Q_INVOKABLE static QString transportMediumAsString(QNetworkInformation::TransportMedium transportMedium);

    static QString backend();
    static QString reachability();
    static QString transportMedium();
    static QString captivePortal();
    static QString metered();

    static QString ipv4Address();
    static QString ethernetIPv4Address();
    static QString wifiIPv4Address();
    static QString usbIPv4Address();
    static QString localHostName();
    static QString localDomainName();

    Q_INVOKABLE static bool waitForReachability(QNetworkInformation::Reachability reachability, int timeout=30000);

signals:
    void ipv4AddressChanged();
    void reachabilityChanged(QNetworkInformation::Reachability reachability);
    void transportMediumChanged(QNetworkInformation::TransportMedium current);
    void captivePortalChanged();
    void meteredChanged();
};

#endif // NETWORKINFORMATION_H
