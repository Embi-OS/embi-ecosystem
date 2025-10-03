#ifndef HOSTNAMESETTINGS_H
#define HOSTNAMESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

class QTranslatorLoader;

class HostnameSettings : public QObject,
                         public QQmlSingleton<HostnameSettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canSetHostname READ canSetHostname   CONSTANT FINAL)
    Q_PROPERTY(QString hostname    READ getHostname      WRITE setHostname NOTIFY hostnameChanged FINAL)
    Q_PROPERTY(QString staticHostname READ getStaticHostname NOTIFY hostnameChanged FINAL)
    Q_PROPERTY(QString prettyHostname READ getPrettyHostname NOTIFY hostnameChanged FINAL)
    Q_PROPERTY(QString iconName READ getIconName CONSTANT FINAL)
    Q_PROPERTY(QString chassis READ getChassis CONSTANT FINAL)
    Q_PROPERTY(QString kernelName READ getKernelName CONSTANT FINAL)
    Q_PROPERTY(QString kernelRelease READ getKernelRelease CONSTANT FINAL)
    Q_PROPERTY(QString kernelVersion READ getKernelVersion CONSTANT FINAL)
    Q_PROPERTY(QString operatingSystemPrettyName READ getOperatingSystemPrettyName CONSTANT FINAL)
    Q_PROPERTY(QString operatingSystemCPEName READ getOperatingSystemCPEName CONSTANT FINAL)
    Q_PROPERTY(QString hardwareVendor READ getHardwareVendor CONSTANT FINAL)
    Q_PROPERTY(QString hardwareModel READ getHardwareModel CONSTANT FINAL)

protected:
    friend QQmlSingleton<HostnameSettings>;
    explicit HostnameSettings(QObject *parent = nullptr);

public:
    static bool canSetHostname();

    QString getHostname() const;
    QString getStaticHostname() const;
    QString getPrettyHostname() const;
    QString getIconName() const;
    QString getChassis() const;
    QString getKernelName() const;
    QString getKernelRelease() const;
    QString getKernelVersion() const;
    QString getOperatingSystemPrettyName() const;
    QString getOperatingSystemCPEName() const;
    QString getHardwareVendor() const;
    QString getHardwareModel() const;


public slots:
    void setHostname(const QString& hostname);

signals:
    void hostnameChanged();
};

#endif // HOSTNAMESETTINGS_H
