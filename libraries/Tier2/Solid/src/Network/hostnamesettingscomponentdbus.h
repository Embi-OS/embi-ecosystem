#ifndef HOSTNAMESETTINGSCOMPONENTDBUS_H
#define HOSTNAMESETTINGSCOMPONENTDBUS_H

#include <QtDBus>

class OrgFreedesktopHostname1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(QString Hostname READ hostname CONSTANT FINAL)
    Q_PROPERTY(QString StaticHostname READ staticHostname CONSTANT FINAL)
    Q_PROPERTY(QString PrettyHostname READ prettyHostname CONSTANT FINAL)
    Q_PROPERTY(QString IconName READ iconName CONSTANT FINAL)
    Q_PROPERTY(QString Chassis READ chassis CONSTANT FINAL)
    Q_PROPERTY(QString KernelName READ kernelName CONSTANT FINAL)
    Q_PROPERTY(QString KernelRelease READ kernelRelease CONSTANT FINAL)
    Q_PROPERTY(QString KernelVersion READ kernelVersion CONSTANT FINAL)
    Q_PROPERTY(QString OperatingSystemPrettyName READ operatingSystemPrettyName CONSTANT FINAL)
    Q_PROPERTY(QString OperatingSystemCPEName READ operatingSystemCPEName CONSTANT FINAL)
    Q_PROPERTY(QString HardwareVendor READ hardwareVendor CONSTANT FINAL)
    Q_PROPERTY(QString HardwareModel READ hardwareModel CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.hostname1"; }

    OrgFreedesktopHostname1Interface(const QString &service,
                                     const QString &path,
                                     const QDBusConnection &connection,
                                     QObject *parent = nullptr) :
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    inline QString hostname() const { return qvariant_cast<QString>(property("Hostname"));}
    inline QString staticHostname() const { return qvariant_cast<QString>(property("StaticHostname")); }
    inline QString prettyHostname() const { return qvariant_cast<QString>(property("PrettyHostname")); }
    inline QString iconName() const { return qvariant_cast<QString>(property("IconName")); }
    inline QString chassis() const { return qvariant_cast<QString>(property("Chassis")); }
    inline QString kernelName() const { return qvariant_cast<QString>(property("KernelName")); }
    inline QString kernelRelease() const { return qvariant_cast<QString>(property("KernelRelease")); }
    inline QString kernelVersion() const { return qvariant_cast<QString>(property("KernelVersion")); }
    inline QString operatingSystemPrettyName() const { return qvariant_cast<QString>(property("OperatingSystemPrettyName")); }
    inline QString operatingSystemCPEName() const { return qvariant_cast<QString>(property("OperatingSystemCPEName")); }
    inline QString hardwareVendor() const { return qvariant_cast<QString>(property("HardwareVendor")); }
    inline QString hardwareModel() const { return qvariant_cast<QString>(property("HardwareModel")); }

public Q_SLOTS:  // METHODS
    inline QDBusMessage SetHostname(const QString &hostname, bool userInteraction) {
        QVariantList args;
        args << QVariant::fromValue(hostname) << QVariant::fromValue(userInteraction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetHostname"), args);
    }

    inline QDBusMessage SetStaticHostname(const QString &hostname, bool userInteraction) {
        QVariantList args;
        args << QVariant::fromValue(hostname) << QVariant::fromValue(userInteraction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetStaticHostname"), args);
    }

    inline QDBusMessage SetPrettyHostname(const QString &hostname, bool userInteraction) {
        QVariantList args;
        args << QVariant::fromValue(hostname) << QVariant::fromValue(userInteraction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetPrettyHostname"), args);
    }

    inline QDBusMessage SetIconName(const QString &icon, bool userInteraction) {
        QVariantList args;
        args << QVariant::fromValue(icon) << QVariant::fromValue(userInteraction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetIconName"), args);
    }

    inline QDBusMessage SetChassis(const QString &chassis, bool userInteraction) {
        QVariantList args;
        args << QVariant::fromValue(chassis) << QVariant::fromValue(userInteraction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetChassis"), args);
    }
};

namespace org {
  namespace freedesktop {
    typedef ::OrgFreedesktopHostname1Interface hostname1;
  }
}

#include "abstracthostnamecomponent.h"
class HostnameSettingsComponentDBus : public AbstractHostnameComponent
{
public:
    HostnameSettingsComponentDBus();

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

#endif // HOSTNAMESETTINGSCOMPONENTDBUS_H
