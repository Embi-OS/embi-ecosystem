#ifndef HOSTNAMEBACKENDDBUS_H
#define HOSTNAMEBACKENDDBUS_H

#include "hostnamebackend.h"

class OrgFreedesktopHostname1Interface;
class HostnameBackendDbus final : public HostnameBackend
{
public:
    explicit HostnameBackendDbus(QObject* parent=nullptr);

    int getCapabilities() override;

    QString getHostname() const override;
    bool setHostname(const QString& locale) override;

    QString getStaticHostname() const override;
    QString getPrettyHostname() const override;
    QString getIconName() const override;
    QString getChassis() const override;
    QString getKernelName() const override;
    QString getKernelRelease() const override;
    QString getKernelVersion() const override;
    QString getOperatingSystemPrettyName() const override;
    QString getOperatingSystemCPEName() const override;
    QString getHardwareVendor() const override;
    QString getHardwareModel() const override;

private:
    OrgFreedesktopHostname1Interface *m_interface;
};

#endif // HOSTNAMEBACKENDDBUS_H
