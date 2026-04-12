#include "hostnamebackenddbus.h"

#include "hostname1interface.h"
#include <QtDBus/QDBusConnection>

HostnameBackendDbus::HostnameBackendDbus(QObject* parent):
    HostnameBackend(parent),
    m_interface(new OrgFreedesktopHostname1Interface(QStringLiteral("org.freedesktop.hostname1"),
                                                     QStringLiteral("/org/freedesktop/hostname1"),
                                                      QDBusConnection::systemBus(), this))
{

}

int HostnameBackendDbus::getCapabilities()
{
    return Capabilities::Hostname;
}

QString HostnameBackendDbus::getHostname() const
{
    return m_interface->hostname();
}

bool HostnameBackendDbus::setHostname(const QString &hostname)
{
    do {
        m_interface->SetHostname(hostname, true);
    } while(getHostname() != hostname);

    return true;
}

QString HostnameBackendDbus::getStaticHostname() const
{
    return m_interface->staticHostname();
}

QString HostnameBackendDbus::getPrettyHostname() const
{
    return m_interface->prettyHostname();
}

QString HostnameBackendDbus::getIconName() const
{
    return m_interface->iconName();
}

QString HostnameBackendDbus::getChassis() const
{
    return m_interface->chassis();
}

QString HostnameBackendDbus::getKernelName() const
{
    return m_interface->kernelName();
}

QString HostnameBackendDbus::getKernelRelease() const
{
    return m_interface->kernelRelease();
}

QString HostnameBackendDbus::getKernelVersion() const
{
    return m_interface->kernelVersion();
}

QString HostnameBackendDbus::getOperatingSystemPrettyName() const
{
    return m_interface->operatingSystemPrettyName();
}

QString HostnameBackendDbus::getOperatingSystemCPEName() const
{
    return m_interface->operatingSystemCPEName();
}

QString HostnameBackendDbus::getHardwareVendor() const
{
    return m_interface->hardwareVendor();
}

QString HostnameBackendDbus::getHardwareModel() const
{
    return m_interface->hardwareModel();
}
