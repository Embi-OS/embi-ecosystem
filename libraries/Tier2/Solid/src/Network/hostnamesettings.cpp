#include "hostnamesettings.h"
#include "solid_log.h"

#include <QHostInfo>

#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
#include "hostnamesettingscomponentdbus.h"
#endif
#include "abstracthostnamecomponent.h"

static AbstractHostnameComponent* getComponent()
{
    static AbstractHostnameComponent* instance = nullptr;

    if(!instance)
    {
#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
        instance = new HostnameSettingsComponentDBus();
#endif
    }

    return instance;
}

HostnameSettings::HostnameSettings(QObject *parent) :
    QObject(parent)
{
    if(!getComponent()) {
        SOLIDLOG_WARNING()<<"Could not find a hostnamesettings component matching this platform";
    }
}

bool HostnameSettings::canSetHostname()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetHostname();
}

QString HostnameSettings::getHostname() const
{
    return QHostInfo::localHostName();
}

void HostnameSettings::setHostname(const QString& hostname)
{
    if(!canSetHostname())
    {
        SOLIDLOG_WARNING()<<"Cannot set hostname";
        return;
    }

    if(getComponent()->setHostname(hostname))
        emit this->hostnameChanged();
}

QString HostnameSettings::getStaticHostname() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getStaticHostname();
}
QString HostnameSettings::getPrettyHostname() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getPrettyHostname();
}
QString HostnameSettings::getIconName() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getIconName();
}
QString HostnameSettings::getChassis() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getChassis();
}
QString HostnameSettings::getKernelName() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getKernelName();
}
QString HostnameSettings::getKernelRelease() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getKernelRelease();
}
QString HostnameSettings::getKernelVersion() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getKernelVersion();
}
QString HostnameSettings::getOperatingSystemPrettyName() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getOperatingSystemPrettyName();
}
QString HostnameSettings::getOperatingSystemCPEName() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getOperatingSystemCPEName();
}
QString HostnameSettings::getHardwareVendor() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getHardwareVendor();
}
QString HostnameSettings::getHardwareModel() const
{
    if(!getComponent())
        return QString();
    return getComponent()->getHardwareModel();
}
