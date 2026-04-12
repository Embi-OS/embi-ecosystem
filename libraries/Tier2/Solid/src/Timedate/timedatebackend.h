#ifndef TIMEDATEBACKEND_H
#define TIMEDATEBACKEND_H

#include <QDefs>

class TimedateBackend: public QObject
{
public:
    enum Capabilities
    {
        Timezone        = ( 1<<0 ),
        Ntp             = ( 1<<1 ),
        SystemDateTime  = ( 1<<2 ),
        NtpServer       = ( 1<<3 ),
    };

    explicit TimedateBackend(QObject* parent=nullptr): QObject(parent) {};

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetTimezone() { return hasCapability(Capabilities::Timezone); }
    bool canSetNtp() { return hasCapability(Capabilities::Ntp); }
    bool canSetSystemDateTime() { return hasCapability(Capabilities::SystemDateTime); }
    bool canSetNtpServer() { return hasCapability(Capabilities::NtpServer); }

    virtual int getCapabilities() = 0;

    virtual QString getTimezone() const { return QTimeZone::systemTimeZoneId(); };
    virtual bool setTimezone(const QString& aTimeZone) { Q_UNUSED(aTimeZone); return false; };

    virtual bool getNtp() const { return false; };
    virtual bool setNtp(bool aNtp) { Q_UNUSED(aNtp); return false; };

    virtual bool setSystemTime(const QDateTime& aTime) { Q_UNUSED(aTime); return false; };

    virtual QString getNtpServer() const { return QString(); };
    virtual bool setNtpServer(const QString& ntpServer) { Q_UNUSED(ntpServer); return false; };

    virtual QString getServerName() const { return QString(); };

};

#endif // TIMEDATEBACKEND_H
