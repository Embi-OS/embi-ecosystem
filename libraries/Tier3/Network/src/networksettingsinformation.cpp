#include "networksettingsinformation.h"
#include "solid_log.h"

#include "Network/networksettingsmanager.h"
#include "qmodelmatcher.h"

NetworkSettingsInformation::NetworkSettingsInformation(QObject *parent) :
   QObject(parent)
{
    QModelMatcher* stateMatcher = new QModelMatcher(this);
    stateMatcher->setSourceModel(NetworkSettingsManager::Get()->interfaces());
    stateMatcher->setRoleName("state");
    stateMatcher->setValue(NetworkSettingsState::Online);
    stateMatcher->setDelayed(true);
    connect(stateMatcher, &QModelMatcher::countChanged, this, &NetworkSettingsInformation::evaluateInterfaces);
    stateMatcher->invalidate();
}

NetworkSettingsInformation* NetworkSettingsInformation::netInfo()
{
    return Get();
}

bool NetworkSettingsInformation::isInitialized() const
{
    return NetworkSettingsManager::Get()->interfaces()->rowCount()>0;
}

bool NetworkSettingsInformation::supports(QNetworkInformation::Features features) const
{
    return supportedFeatures().testFlags(features);
}

QNetworkInformation::Features NetworkSettingsInformation::supportedFeatures() const
{
#ifdef Q_OS_BOOT2QT
    return QNetworkInformation::Features(QNetworkInformation::Feature::Reachability | QNetworkInformation::Feature::TransportMedium);
#else
    if(isInitialized())
        return QNetworkInformation::Features(QNetworkInformation::Feature::Reachability | QNetworkInformation::Feature::TransportMedium);
    return QNetworkInformation::Features();
#endif
}

void NetworkSettingsInformation::evaluateInterfaces()
{
    emit this->transportMediumChanged(transportMedium());
    emit this->reachabilityChanged(reachability());
}

QNetworkInformation::Reachability NetworkSettingsInformation::reachability() const
{
    const QList<NetworkSettingsInterface*> interfaces = NetworkSettingsManager::Get()->interfaces()->getModel();
    for (NetworkSettingsInterface* iface: interfaces) {
        if (iface->state()==NetworkSettingsState::Online) {
            return QNetworkInformation::Reachability::Online;
        }
    }

    return QNetworkInformation::Reachability::Unknown;
}

QNetworkInformation::TransportMedium NetworkSettingsInformation::transportMedium() const
{
    const QList<NetworkSettingsInterface*> interfaces = NetworkSettingsManager::Get()->interfaces()->getModel();
    for (NetworkSettingsInterface* iface: interfaces) {
        if (iface->state()==NetworkSettingsState::Online) {
            switch (iface->type()) {
            case NetworkSettingsType::Wired:        return QNetworkInformation::TransportMedium::Ethernet;
            case NetworkSettingsType::Wifi:         return QNetworkInformation::TransportMedium::WiFi;
            case NetworkSettingsType::Bluetooth:    return QNetworkInformation::TransportMedium::Bluetooth;
            default: break;
            }
        }
    }

    return QNetworkInformation::TransportMedium::Unknown;
}

bool NetworkSettingsInformation::waitForReachability(QNetworkInformation::Reachability reachability, int timeout)
{
    if(!netInfo() || !netInfo()->supports(QNetworkInformation::Feature::Reachability))
    {
        SOLIDLOG_WARNING()<<"NetworkSettingsInformation reachability can not be determined";
        return true;
    }

    if(netInfo()->reachability()>=reachability)
    {
        SOLIDLOG_INFO()<<"NetworkSettingsInformation reachability is"<<netInfo()->reachability();
        return true;
    }

    QElapsedTimer elapsed;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    QEventLoop loop;

    auto conn1 = connect(netInfo(), &NetworkSettingsInformation::reachabilityChanged, netInfo(), [&](QNetworkInformation::Reachability newReachability) {
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
    QObject::disconnect(conn3);

    if(result==0)
        SOLIDLOG_INFO()<<"NetworkSettingsInformation reachability changed to"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";
    else
        SOLIDLOG_WARNING()<<"NetworkSettingsInformation reachability is still"<<netInfo()->reachability()<<"after"<<elapsed.nsecsElapsed()/1000000.0<<"ms";

    return result==0;
}
