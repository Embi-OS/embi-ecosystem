#include "hostnamesettingscomponentdbus.h"

HostnameSettingsComponentDBus::HostnameSettingsComponentDBus()
{
    m_interface = new OrgFreedesktopHostname1Interface(QStringLiteral("org.freedesktop.hostname1"),
                                                       QStringLiteral("/org/freedesktop/hostname1"),
                                                       QDBusConnection::systemBus());
}

int HostnameSettingsComponentDBus::getCapabilities()
{
    return Capabilities::Hostname;
}

QString HostnameSettingsComponentDBus::getHostname() const
{
    if(!m_interface)
        return QString();
    return m_interface->hostname();
}

bool HostnameSettingsComponentDBus::setHostname(const QString& hostname)
{
    if(!m_interface)
        return false;

    do {
        m_interface->SetHostname(hostname, true);
    } while(getHostname() != hostname);

    return true;
}

QString HostnameSettingsComponentDBus::getStaticHostname() const
{
    if(!m_interface)
        return QString();
    return m_interface->staticHostname();
}
QString HostnameSettingsComponentDBus::getPrettyHostname() const
{
    if(!m_interface)
        return QString();
    return m_interface->prettyHostname();
}
QString HostnameSettingsComponentDBus::getIconName() const
{
    if(!m_interface)
        return QString();
    return m_interface->iconName();
}
QString HostnameSettingsComponentDBus::getChassis() const
{
    if(!m_interface)
        return QString();
    return m_interface->chassis();
}
QString HostnameSettingsComponentDBus::getKernelName() const
{
    if(!m_interface)
        return QString();
    return m_interface->kernelName();
}
QString HostnameSettingsComponentDBus::getKernelRelease() const
{
    if(!m_interface)
        return QString();
    return m_interface->kernelRelease();
}
QString HostnameSettingsComponentDBus::getKernelVersion() const
{
    if(!m_interface)
        return QString();
    return m_interface->kernelVersion();
}
QString HostnameSettingsComponentDBus::getOperatingSystemPrettyName() const
{
    if(!m_interface)
        return QString();
    return m_interface->operatingSystemPrettyName();
}
QString HostnameSettingsComponentDBus::getOperatingSystemCPEName() const
{
    if(!m_interface)
        return QString();
    return m_interface->operatingSystemCPEName();
}
QString HostnameSettingsComponentDBus::getHardwareVendor() const
{
    if(!m_interface)
        return QString();
    return m_interface->hardwareVendor();
}
QString HostnameSettingsComponentDBus::getHardwareModel() const
{
    if(!m_interface)
        return QString();
    return m_interface->hardwareModel();
}
