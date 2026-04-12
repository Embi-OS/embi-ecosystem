#include "networkinformation.h"
#include "solid_log.h"

#include <QHostInfo>
#include <QNetworkInterface>

// TODO: Qt6 QNetworkInformation

NetworkInformation::NetworkInformation(QObject *parent) :
    QObject(parent)
{
    if(netInfo())
    {
        connect(netInfo(), &QNetworkInformation::isBehindCaptivePortalChanged, this, &NetworkInformation::captivePortalChanged);
        connect(netInfo(), &QNetworkInformation::isMeteredChanged, this, &NetworkInformation::meteredChanged);
        connect(netInfo(), &QNetworkInformation::reachabilityChanged, this, &NetworkInformation::reachabilityChanged);
        connect(netInfo(), &QNetworkInformation::transportMediumChanged, this, &NetworkInformation::transportMediumChanged);

        connect(netInfo(), &QNetworkInformation::isBehindCaptivePortalChanged, this, &NetworkInformation::informationsChanged);
        connect(netInfo(), &QNetworkInformation::isMeteredChanged, this, &NetworkInformation::informationsChanged);
        connect(netInfo(), &QNetworkInformation::reachabilityChanged, this, &NetworkInformation::informationsChanged);
        connect(netInfo(), &QNetworkInformation::transportMediumChanged, this, &NetworkInformation::informationsChanged);
    }
}

QNetworkInformation* NetworkInformation::netInfo()
{
    static QNetworkInformation* s_netInfoInstance = []() {
        QNetworkInformation::loadDefaultBackend();
        QNetworkInformation* instance = QNetworkInformation::instance();
        if(instance)
            QJSEngine::setObjectOwnership(instance, QJSEngine::CppOwnership);
        return instance;
    }();

    return s_netInfoInstance;
}

QString NetworkInformation::reachabilityAsString(QNetworkInformation::Reachability reachability)
{
    QString ret = "?????";
    switch (reachability) {
    case QNetworkInformation::Reachability::Unknown: ret = "UNKNOWN"; break;
    case QNetworkInformation::Reachability::Disconnected: ret = "DISCONNECTED"; break;
    case QNetworkInformation::Reachability::Local: ret = "LOCAL"; break;
    case QNetworkInformation::Reachability::Site: ret = "SITE"; break;
    case QNetworkInformation::Reachability::Online: ret = "ONLINE"; break;
    default: break;
    }

    // reachability.append("\n(Can be UNKNOWN, DISCONNECTED, LOCAL, SITE, ONLINE)");

    return ret;
}

QString NetworkInformation::transportMediumAsString(QNetworkInformation::TransportMedium transportMedium)
{
    QString ret = "?????";
    switch (transportMedium) {
    case QNetworkInformation::TransportMedium::Unknown: ret = "UNKNOWN"; break;
    case QNetworkInformation::TransportMedium::Ethernet: ret = "ETHERNET"; break;
    case QNetworkInformation::TransportMedium::Cellular: ret = "CELLULAR"; break;
    case QNetworkInformation::TransportMedium::WiFi: ret = "WIFI"; break;
    case QNetworkInformation::TransportMedium::Bluetooth: ret = "BLUETOOTH"; break;
    default: break;
    }

    // transportMedium.append("\n(Can be UNKNOWN, ETHERNET, CELLULAR, WIFI, BLUETOOTH)");

    return ret;
}

QString NetworkInformation::backend()
{
    if(!netInfo())
        return "N/A";

    QString backendName = netInfo()->backendName();

    // backendName.append("\n(Can be networklistmanager (Windows), scnetworkreachability (Apple macOS, iOS), android (Android), networkmanager (Linux))");

    return backendName;
}
QString NetworkInformation::reachability()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::Reachability))
        return "N/A";

    return NetworkInformation::reachabilityAsString(netInfo()->reachability());
}
QString NetworkInformation::transportMedium()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::TransportMedium))
        return "N/A";

    return NetworkInformation::transportMediumAsString(netInfo()->transportMedium());
}
QString NetworkInformation::captivePortal()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::CaptivePortal))
        return "N/A";

    return netInfo()->isBehindCaptivePortal() ? "YES" : "NO";
}
QString NetworkInformation::metered()
{
    if(!netInfo() || !netInfo()->supportedFeatures().testFlag(QNetworkInformation::Feature::Metered))
        return "N/A";

    return netInfo()->isMetered() ? "YES" : "NO";
}

QString NetworkInformation::getIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()!=QNetworkInterface::Loopback && !iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString NetworkInformation::getEthernetIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Ethernet && !iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString NetworkInformation::getWifiIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Wifi && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString NetworkInformation::getUsbIPv4Address()
{
#ifndef QT_NO_NETWORKINTERFACE
    const QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface& iface : ifs)
    {
        if (iface.type()==QNetworkInterface::Ethernet && iface.name().contains("usb") && iface.isValid() && iface.flags() & QNetworkInterface::IsUp)
        {
            const QList<QNetworkAddressEntry> entries = iface.addressEntries();
            for(const QNetworkAddressEntry& entry : entries)
            {
                if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    return entry.ip().toString();
            }
        }
    }
#endif
    return "N/A";
}

QString NetworkInformation::getHostname()
{
    return QHostInfo::localHostName();
}

QString NetworkInformation::getDomain()
{
    return QHostInfo::localDomainName();
}

bool NetworkInformation::waitForReachability(QNetworkInformation::Reachability reachability, int timeout)
{
    if(!netInfo() || !netInfo()->supports(QNetworkInformation::Feature::Reachability))
    {
        SOLIDLOG_WARNING()<<"NetworkInformation reachability can not be determined";
        return true;
    }

    if(netInfo()->reachability()>=reachability)
    {
        SOLIDLOG_INFO()<<"NetworkInformation reachability is"<<netInfo()->reachability();
        return true;
    }

    QElapsedTimer elapsed;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    QEventLoop loop;

    auto conn1 = connect(netInfo(), &QNetworkInformation::reachabilityChanged, netInfo(), [&](QNetworkInformation::Reachability newReachability) {
        if(!loop.isRunning())
            return;
        if(newReachability>=reachability)
            loop.exit(0);
    });

    auto conn2 = connect(&timer, &QTimer::timeout, netInfo(), [&]() {
        if(!loop.isRunning())
            return;
        if(netInfo()->reachability()>=reachability)
            loop.exit(0);
        else
            loop.exit(1);
    });

    QTimer debug;
    debug.setInterval(1000);
    debug.setSingleShot(false);
    auto conn3 = connect(&debug, &QTimer::timeout, netInfo(), [&]() {
        SOLIDLOG_INFO()<<"Wait for network reachability - remaining time:"<<qRound(timer.remainingTime()/1000.0)<<"sec";
    });

    elapsed.start();
    timer.start();
    debug.start();
    int result = loop.exec();

    QObject::disconnect(conn1);
    QObject::disconnect(conn2);

    if(result==0)
        SOLIDLOG_INFO()<<"NetworkInformation reachability changed to"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";
    else
        SOLIDLOG_WARNING()<<"NetworkInformation reachability is still"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";

    return result==0;
}
