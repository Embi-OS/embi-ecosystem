#ifndef HOSTNAMEBACKEND_H
#define HOSTNAMEBACKEND_H

#include <QDefs>
#include <QHostInfo>

class HostnameBackend: public QObject
{
public:
    explicit HostnameBackend(QObject* parent=nullptr): QObject(parent) {};

    enum Capabilities
    {
        Hostname = 0x01
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetHostname() { return hasCapability(Capabilities::Hostname); }

    virtual int getCapabilities() = 0;

    virtual QString getHostname() const { return QHostInfo::localHostName(); };
    virtual bool setHostname(const QString& hostname) { Q_UNUSED(hostname); return false; };

    virtual QString getStaticHostname() const { return QHostInfo::localHostName(); };
    virtual QString getPrettyHostname() const { return QHostInfo::localHostName(); };
    virtual QString getIconName() const { return QString(); };
    virtual QString getChassis() const { return QString(); };
    virtual QString getKernelName() const { return QSysInfo::kernelType(); };
    virtual QString getKernelRelease() const { return QSysInfo::kernelVersion(); };
    virtual QString getKernelVersion() const { return QSysInfo::kernelVersion(); };
    virtual QString getOperatingSystemPrettyName() const { return QSysInfo::prettyProductName(); };
    virtual QString getOperatingSystemCPEName() const { return QString(); };
    virtual QString getHardwareVendor() const { return QString(); };
    virtual QString getHardwareModel() const { return QSysInfo::currentCpuArchitecture(); };

};

#endif // HOSTNAMEBACKEND_H
