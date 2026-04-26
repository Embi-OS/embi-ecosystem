#ifndef NETWORKINTERFACEHELPER_H
#define NETWORKINTERFACEHELPER_H

#include <QHostAddress>
#include <QNetworkInterface>
#include <QString>

class NetworkInterfaceHelper final
{
public:
    NetworkInterfaceHelper() = delete;

    static QString primaryIpv4Address();
    static QString ethernetIpv4Address();
    static QString wifiIpv4Address();
    static QString usbIpv4Address();
    static QString primaryIpv4Address(const QNetworkInterface &interface);

private:
    static QString findBestIpv4Address(const QList<QNetworkInterface> &interfaces,
                                       const std::function<bool(const QNetworkInterface&)> &filter);
    static int interfacePriority(const QNetworkInterface &interface);

    static bool containsAny(const QString &value, std::initializer_list<const char*> needles);
    static bool startsWithAny(const QString &value, std::initializer_list<const char*> prefixes);
    static QString interfaceText(const QNetworkInterface &interface);

    static bool isLoopbackInterface(const QNetworkInterface &interface);
    static bool isVirtualInterface(const QNetworkInterface &interface);
    static bool isWifiInterface(const QNetworkInterface &interface);
    static bool isUsbInterface(const QNetworkInterface &interface);
    static bool isEthernetInterface(const QNetworkInterface &interface);

    static bool isUsableIpv4(const QHostAddress &address);
    static int ipv4AddressScore(const QNetworkAddressEntry &entry);
    static bool isUsableInterface(const QNetworkInterface &interface);
};

#endif // NETWORKINTERFACEHELPER_H
