#include "hostnamesettings.h"
#include "solid_log.h"

#include <QHostInfo>

#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
#include "hostname1interface.h"
#endif

HostnameSettings::HostnameSettings(QObject *parent) :
    QObject(parent),
#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
    m_interface(new OrgFreedesktopHostname1Interface(QStringLiteral("org.freedesktop.hostname1"),
                                                     QStringLiteral("/org/freedesktop/hostname1"),
                                                     QDBusConnection::systemBus(), this))
#endif
{
    if (!m_interface) {
        SOLIDLOG_WARNING() << "Could not find a hostnamesettings backend matching this platform";
    }
}

int HostnameSettings::getCapabilities() const
{
    if (!m_interface)
        return 0;
    return Capabilities::Hostname;
}

QString HostnameSettings::getHostname() const
{
    if(!m_interface)
        return QHostInfo::localHostName();
    return m_interface->hostname();
}

bool HostnameSettings::setHostname(const QString& hostname)
{
    if(!m_interface || !canSetHostname())
    {
        SOLIDLOG_WARNING()<<"Cannot set hostname";
        return false;
    }

    do {
        m_interface->SetHostname(hostname, true);
    } while(getHostname() != hostname);

    emit this->hostnameChanged();

    return true;
}

QString HostnameSettings::getStaticHostname() const
{
    if(!m_interface)
        return QHostInfo::localHostName();
    return m_interface->staticHostname();
}

QString HostnameSettings::getPrettyHostname() const
{
    if(!m_interface)
        return QHostInfo::localHostName();
    return m_interface->prettyHostname();
}

QString HostnameSettings::getIconName() const
{
    if(!m_interface)
        return QString();
    return m_interface->iconName();
}

QString HostnameSettings::getChassis() const
{
    if(!m_interface)
        return QString();
    return m_interface->chassis();
}

QString HostnameSettings::getKernelName() const
{
    if(!m_interface)
        return QSysInfo::kernelType();
    return m_interface->kernelName();
}

QString HostnameSettings::getKernelRelease() const
{
    if(!m_interface)
        return QSysInfo::kernelVersion();
    return m_interface->kernelRelease();
}

QString HostnameSettings::getKernelVersion() const
{
    if(!m_interface)
        return QSysInfo::kernelVersion();
    return m_interface->kernelVersion();
}

QString HostnameSettings::getOperatingSystemPrettyName() const
{
    if(!m_interface)
        return QSysInfo::prettyProductName();
    return m_interface->operatingSystemPrettyName();
}

QString HostnameSettings::getOperatingSystemCPEName() const
{
    if(!m_interface)
        return QString();
    return m_interface->operatingSystemCPEName();
}

QString HostnameSettings::getHardwareVendor() const
{
    if(!m_interface)
        return QString();
    return m_interface->hardwareVendor();
}

QString HostnameSettings::getHardwareModel() const
{
    if(!m_interface)
        return QSysInfo::currentCpuArchitecture();
    return m_interface->hardwareModel();
}
