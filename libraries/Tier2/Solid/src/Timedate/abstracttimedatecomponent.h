#ifndef ABSTRACTTIMEDATECOMPONENT_H
#define ABSTRACTTIMEDATECOMPONENT_H

#include <QString>
#include <QDateTime>

class AbstractTimedateComponent
{
public:
    AbstractTimedateComponent() {};

    enum Capabilities
    {
        Timezone = 0x01,
        Ntp = 0x02,
        SystemDateTime = 0x04,
        NtpServer = 0x08
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetTimezone() { return hasCapability(Capabilities::Timezone); }
    bool canSetNtp() { return hasCapability(Capabilities::Ntp); }
    bool canSetSystemDateTime() { return hasCapability(Capabilities::SystemDateTime); }
    bool canSetNtpServer() { return hasCapability(Capabilities::NtpServer); }

    virtual int getCapabilities() = 0;

    virtual QString getTimezone() const { return QString(); };
    virtual bool setTimezone(const QString& aTimeZone) { Q_UNUSED(aTimeZone); return false; };

    virtual bool getNtp() const { return false; };
    virtual bool setNtp(bool aNtp) { Q_UNUSED(aNtp); return false; };

    virtual bool setSystemTime(const QDateTime& aTime) { Q_UNUSED(aTime); return false; };

    virtual QString getNtpServer() const { return QString(); };
    virtual bool setNtpServer(const QString& ntpServer) { Q_UNUSED(ntpServer); return false; };

    virtual QString getServerName() const { return QString(); };
};

#endif // ABSTRACTTIMEDATECOMPONENT_H
