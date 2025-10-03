#ifndef ABSTRACTHOSTNAMECOMPONENT_H
#define ABSTRACTHOSTNAMECOMPONENT_H

#include <QString>

class AbstractHostnameComponent
{
public:
    AbstractHostnameComponent() {};

    enum Capabilities
    {
        Hostname = 0x01
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetHostname() { return hasCapability(Capabilities::Hostname); }

    virtual int getCapabilities() = 0;

    virtual QString getHostname() const { return QString(); };
    virtual bool setHostname(const QString& hostname) { Q_UNUSED(hostname); return false; };

    virtual QString getStaticHostname() const { return QString(); };
    virtual QString getPrettyHostname() const { return QString(); };
    virtual QString getIconName() const { return QString(); };
    virtual QString getChassis() const { return QString(); };
    virtual QString getKernelName() const { return QString(); };
    virtual QString getKernelRelease() const { return QString(); };
    virtual QString getKernelVersion() const { return QString(); };
    virtual QString getOperatingSystemPrettyName() const { return QString(); };
    virtual QString getOperatingSystemCPEName() const { return QString(); };
    virtual QString getHardwareVendor() const { return QString(); };
    virtual QString getHardwareModel() const { return QString(); };
};

#endif // ABSTRACTHOSTNAMECOMPONENT_H
