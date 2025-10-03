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
        Timeservers = 0x08
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetTimezone() { return hasCapability(Capabilities::Timezone); }
    bool canSetNtp() { return hasCapability(Capabilities::Ntp); }
    bool canSetSystemDateTime() { return hasCapability(Capabilities::SystemDateTime); }
    bool canSetTimeservers() { return hasCapability(Capabilities::Timeservers); }

    virtual int getCapabilities() = 0;

    virtual QString getTimezone() const { return QString(); };
    virtual bool setTimezone(const QString& aTimeZone) { Q_UNUSED(aTimeZone); return false; };

    virtual bool getNtp() const { return false; };
    virtual bool setNtp(bool aNtp) { Q_UNUSED(aNtp); return false; };

    virtual bool setSystemTime(const QDateTime& aTime) { Q_UNUSED(aTime); return false; };

    virtual QString getTimeservers() const { return QString(); };
    virtual bool setTimeservers(const QString& timeservers) { Q_UNUSED(timeservers); return false; };

    virtual QString serverName() const { return QString(); };
    virtual QString serverAddress() const { return QString(); };
    virtual int pollIntervalMinUSec() const { return 0; };
    virtual int pollIntervalMaxUSec() const { return 0; };
    virtual int frequency() const { return 0; };
};

#endif // ABSTRACTTIMEDATECOMPONENT_H
