#include "networkinterfacehelper.h"

QString NetworkInterfaceHelper::primaryIpv4Address()
{
    return findBestIpv4Address(QNetworkInterface::allInterfaces(), [](const QNetworkInterface &interface) {
        return isUsableInterface(interface) && !isVirtualInterface(interface);
    });
}

QString NetworkInterfaceHelper::ethernetIpv4Address()
{
    return findBestIpv4Address(QNetworkInterface::allInterfaces(), [](const QNetworkInterface &interface) {
        return isUsableInterface(interface) && isEthernetInterface(interface) && !isUsbInterface(interface);
    });
}

QString NetworkInterfaceHelper::wifiIpv4Address()
{
    return findBestIpv4Address(QNetworkInterface::allInterfaces(), [](const QNetworkInterface &interface) {
        return isUsableInterface(interface) && isWifiInterface(interface);
    });
}

QString NetworkInterfaceHelper::usbIpv4Address()
{
    return findBestIpv4Address(QNetworkInterface::allInterfaces(), [](const QNetworkInterface &interface) {
        return isUsableInterface(interface) && isUsbInterface(interface);
    });
}

QString NetworkInterfaceHelper::primaryIpv4Address(const QNetworkInterface &interface)
{
    int bestScore = -1;
    QString bestAddress;

    for(const QNetworkAddressEntry &entry : interface.addressEntries())
    {
        const int score = ipv4AddressScore(entry);
        if(score > bestScore)
        {
            bestScore = score;
            bestAddress = entry.ip().toString();
        }
    }

    return bestAddress;
}

QString NetworkInterfaceHelper::findBestIpv4Address(const QList<QNetworkInterface> &interfaces,
                                                    const std::function<bool(const QNetworkInterface &)> &filter)
{
    QString bestAddress;
    int bestPriority = std::numeric_limits<int>::min();
    int bestIndex = std::numeric_limits<int>::max();

    for(const QNetworkInterface &interface : interfaces)
    {
        if(!filter(interface))
            continue;

        const QString address = primaryIpv4Address(interface);
        if(address.isEmpty())
            continue;

        const int priority = interfacePriority(interface);
        if(bestAddress.isEmpty() || priority > bestPriority || (priority == bestPriority && interface.index() < bestIndex))
        {
            bestAddress = address;
            bestPriority = priority;
            bestIndex = interface.index();
        }
    }

    return bestAddress;
}

int NetworkInterfaceHelper::interfacePriority(const QNetworkInterface &interface)
{
    if(isEthernetInterface(interface) && !isUsbInterface(interface))
        return 300;
    if(isWifiInterface(interface))
        return 200;
    if(isUsbInterface(interface))
        return 100;
    return 0;
}

bool NetworkInterfaceHelper::containsAny(const QString &value, std::initializer_list<const char*> needles)
{
    for(const char *needle : needles)
    {
        if(value.contains(QLatin1String(needle)))
            return true;
    }

    return false;
}

bool NetworkInterfaceHelper::startsWithAny(const QString &value, std::initializer_list<const char*> prefixes)
{
    for(const char *prefix : prefixes)
    {
        if(value.startsWith(QLatin1String(prefix)))
            return true;
    }

    return false;
}

QString NetworkInterfaceHelper::interfaceText(const QNetworkInterface &interface)
{
    return (interface.name() + QLatin1Char(' ') + interface.humanReadableName()).toLower();
}

bool NetworkInterfaceHelper::isLoopbackInterface(const QNetworkInterface &interface)
{
    return interface.type() == QNetworkInterface::Loopback
           || interface.flags().testFlag(QNetworkInterface::IsLoopBack)
           || interface.name() == QLatin1String("lo");
}

bool NetworkInterfaceHelper::isVirtualInterface(const QNetworkInterface &interface)
{
    const QString text = interfaceText(interface);

    return interface.type() == QNetworkInterface::Virtual
           || startsWithAny(text, {"docker", "br-", "virbr", "veth", "vboxnet", "vmnet", "tailscale", "zt", "tun", "tap", "wg", "ifb", "dummy", "cni", "flannel", "podman", "vethernet"})
           || containsAny(text, {" docker", " bridge", " virtual", " wireguard", " zerotier", " tailscale", " tunnel", " hyper-v", " vmware", " host-only", " npcap", " loopback adapter"});
}

bool NetworkInterfaceHelper::isWifiInterface(const QNetworkInterface &interface)
{
    const QString text = interfaceText(interface);

    return interface.type() == QNetworkInterface::Wifi
           || interface.type() == QNetworkInterface::Ieee80211
           || startsWithAny(text, {"wl", "wlan", "wifi", "ath", "ra"})
           || containsAny(text, {" wi-fi", " wifi", " wireless", " wlan"});
}

bool NetworkInterfaceHelper::isUsbInterface(const QNetworkInterface &interface)
{
    return containsAny(interfaceText(interface), {"usb", "rndis"});
}

bool NetworkInterfaceHelper::isEthernetInterface(const QNetworkInterface &interface)
{
    const QString text = interfaceText(interface);

    return (interface.type() == QNetworkInterface::Ethernet && !isUsbInterface(interface))
           || startsWithAny(text, {"en", "eth", "eno", "ens", "enp", "em", "lan"})
           || containsAny(text, {" ethernet", " local area connection", " lan"});
}

bool NetworkInterfaceHelper::isUsableIpv4(const QHostAddress &address)
{
    return address.protocol() == QAbstractSocket::IPv4Protocol
           && !address.isNull()
           && !address.isLoopback()
           && !address.isLinkLocal()
           && !address.isBroadcast()
           && !address.isMulticast();
}

int NetworkInterfaceHelper::ipv4AddressScore(const QNetworkAddressEntry &entry)
{
    const QHostAddress &address = entry.ip();
    if(!isUsableIpv4(address))
        return -1;

    int score = 100;
    if(!entry.netmask().isNull())
        score += 10;
    if(!entry.broadcast().isNull())
        score += 5;
    if(address.isGlobal())
        score += 2;
    if(address.isPrivateUse())
        score += 1;

    return score;
}

bool NetworkInterfaceHelper::isUsableInterface(const QNetworkInterface &interface)
{
    if(!interface.isValid())
        return false;

    const QNetworkInterface::InterfaceFlags flags = interface.flags();
    return !isLoopbackInterface(interface)
           && flags.testFlag(QNetworkInterface::IsUp)
           && flags.testFlag(QNetworkInterface::IsRunning);
}
