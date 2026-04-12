#include "hostnamesettings.h"
#include "hostnamebackend.h"
#include "solid_log.h"

#include "hostnamebackenddbus.h"

HostnameSettings::HostnameSettings(QObject *parent) :
    QObject(parent),
#if defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX)
    m_backend(new HostnameBackendDbus(this))
#else
    m_backend(nullptr)
#endif
{
    if (!m_backend) {
        SOLIDLOG_WARNING() << "Could not find a hostnamesettings backend matching this platform";
    }
}

bool HostnameSettings::canSetHostname() const
{
    return m_backend && m_backend->canSetHostname();
}

QString HostnameSettings::getHostname() const
{
    if(!m_backend)
        return QHostInfo::localHostName();
    return m_backend->getHostname();
}

void HostnameSettings::setHostname(const QString& hostname)
{
    if(!canSetHostname())
    {
        SOLIDLOG_WARNING()<<"Cannot set hostname";
        return;
    }

    if(m_backend->setHostname(hostname))
        emit this->hostnameChanged();
}

QString HostnameSettings::getStaticHostname() const
{
    if(!m_backend)
        return QHostInfo::localHostName();
    return m_backend->getStaticHostname();
}

QString HostnameSettings::getPrettyHostname() const
{
    if(!m_backend)
        return QHostInfo::localHostName();
    return m_backend->getPrettyHostname();
}

QString HostnameSettings::getIconName() const
{
    if(!m_backend)
        return QString();
    return m_backend->getIconName();
}

QString HostnameSettings::getChassis() const
{
    if(!m_backend)
        return QString();
    return m_backend->getChassis();
}

QString HostnameSettings::getKernelName() const
{
    if(!m_backend)
        return QSysInfo::kernelType();
    return m_backend->getKernelName();
}

QString HostnameSettings::getKernelRelease() const
{
    if(!m_backend)
        return QSysInfo::kernelVersion();
    return m_backend->getKernelRelease();
}

QString HostnameSettings::getKernelVersion() const
{
    if(!m_backend)
        return QSysInfo::kernelVersion();
    return m_backend->getKernelVersion();
}

QString HostnameSettings::getOperatingSystemPrettyName() const
{
    if(!m_backend)
        return QSysInfo::prettyProductName();
    return m_backend->getOperatingSystemPrettyName();
}

QString HostnameSettings::getOperatingSystemCPEName() const
{
    if(!m_backend)
        return QString();
    return m_backend->getOperatingSystemCPEName();
}

QString HostnameSettings::getHardwareVendor() const
{
    if(!m_backend)
        return QString();
    return m_backend->getHardwareVendor();
}

QString HostnameSettings::getHardwareModel() const
{
    if(!m_backend)
        return QSysInfo::currentCpuArchitecture();
    return m_backend->getHardwareModel();
}
