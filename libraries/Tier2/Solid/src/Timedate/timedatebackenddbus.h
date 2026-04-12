#ifndef TIMEDATEBACKENDDBUS_H
#define TIMEDATEBACKENDDBUS_H

#include "timedatebackend.h"

class OrgFreedesktopTimedate1Interface;
class OrgFreedesktopTimesync1Interface;
class TimedateBackendDbus final : public TimedateBackend
{
public:
    explicit TimedateBackendDbus(QObject* parent=nullptr);

    int getCapabilities() override;

    QString getTimezone() const override;
    bool setTimezone(const QString& aTimeZone) override;

    bool getNtp() const override;
    bool setNtp(bool aNtp) override;

    bool setSystemTime(const QDateTime& aTime) override;

    QString getNtpServer() const override;
    bool setNtpServer(const QString& ntpServer) override;

    QString getServerName() const override;

private:
    OrgFreedesktopTimedate1Interface *m_timedateInterface;
    OrgFreedesktopTimesync1Interface *m_timesyncInterface;

};

#endif // TIMEDATEBACKENDDBUS_H
