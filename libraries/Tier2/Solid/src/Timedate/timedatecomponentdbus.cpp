#include "timedatecomponentdbus.h"

#define SETTINGS_FILENAME "/etc/systemd/timesyncd.conf"
TimedateComponentDBus::TimedateComponentDBus() :
    AbstractTimedateComponent()
{
    m_timedateInterface = new OrgFreedesktopTimedate1Interface(QStringLiteral("org.freedesktop.timedate1"),
                                                               QStringLiteral("/org/freedesktop/timedate1"),
                                                               QDBusConnection::systemBus());

    m_timesyncInterface = new OrgFreedesktopTimesync1Interface(QStringLiteral("org.freedesktop.timesync1"),
                                                               QStringLiteral("/org/freedesktop/timesync1"),
                                                               QDBusConnection::systemBus());
}

int TimedateComponentDBus::getCapabilities()
{
    return Capabilities::Timezone |
           Capabilities::Ntp |
           Capabilities::SystemDateTime |
           Capabilities::Timeservers;
}

QString TimedateComponentDBus::getTimezone() const
{
    if(!m_timedateInterface)
        return QString();

    return m_timedateInterface->timezone();
}

bool TimedateComponentDBus::setTimezone(const QString& aTimeZone)
{
    if(!m_timedateInterface)
        return false;

    do {
        m_timedateInterface->SetTimezone(aTimeZone, true);
    } while(getTimezone() != aTimeZone);

    return true;
}

bool TimedateComponentDBus::getNtp() const
{
    if(!m_timedateInterface)
        return false;

    return m_timedateInterface->nTP();
}

bool TimedateComponentDBus::setNtp(const bool aNtp)
{
    if(!m_timedateInterface)
        return false;

    do {
        m_timedateInterface->SetNTP(aNtp, true);
    } while(getNtp() != aNtp);

    return true;
}

QString TimedateComponentDBus::getTimeservers() const
{
    if(!QFile::exists(SETTINGS_FILENAME))
        return QString();

    QSettings settings = QSettings(SETTINGS_FILENAME, QSettings::IniFormat);
    settings.beginGroup("Time");

    return settings.value("NTP", QString()).toString();
}

bool TimedateComponentDBus::setTimeservers(const QString& timeservers)
{
    if(!QFile::exists(SETTINGS_FILENAME))
        return false;

    QSettings settings = QSettings(SETTINGS_FILENAME, QSettings::IniFormat);
    settings.beginGroup("Time");

    if(settings.value("NTP", QString()).toString()==timeservers)
        return true;

    settings.setValue("NTP", timeservers);
    settings.sync();

    return true;
}


bool TimedateComponentDBus::setSystemTime(const QDateTime& aTime)
{
    if(!m_timedateInterface)
        return false;

    qlonglong usecsSinceEpoch = aTime.toMSecsSinceEpoch()*1000;
    m_timedateInterface->SetTime(usecsSinceEpoch, false, true);

    return true;
}

QString TimedateComponentDBus::serverName() const
{
    return m_timesyncInterface ? m_timesyncInterface->serverName() : QString();
}
QString TimedateComponentDBus::serverAddress() const
{
    return m_timesyncInterface ? m_timesyncInterface->serverAddress() : QString();
}
int TimedateComponentDBus::pollIntervalMinUSec() const
{
    return m_timesyncInterface ? m_timesyncInterface->pollIntervalMinUSec() : -1;
}
int TimedateComponentDBus::pollIntervalMaxUSec() const
{
    return m_timesyncInterface ? m_timesyncInterface->pollIntervalMaxUSec() : -1;
}
int TimedateComponentDBus::frequency() const
{
    return m_timesyncInterface ? m_timesyncInterface->frequency() : -1;
}
