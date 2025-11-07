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
           Capabilities::NtpServer;
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

bool TimedateComponentDBus::setSystemTime(const QDateTime& aTime)
{
    if(!m_timedateInterface)
        return false;

    qlonglong usecsSinceEpoch = aTime.toMSecsSinceEpoch()*1000;
    m_timedateInterface->SetTime(usecsSinceEpoch, false, true);

    return true;
}

QString TimedateComponentDBus::getNtpServer() const
{
    if(!QFile::exists(SETTINGS_FILENAME))
        return QString();

    QFile file(SETTINGS_FILENAME);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString(); // Could not open file
    }

    QTextStream in(&file);
    QString ntpValue;
    static const QRegularExpression ntpRegex(R"(^\s*#?\s*NTP\s*=\s*(.*)\s*$)");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = ntpRegex.match(line);
        if (match.hasMatch()) {
            ntpValue = match.captured(1).trimmed();
            break;
        }
    }

    file.close();
    return ntpValue;
}

bool TimedateComponentDBus::setNtpServer(const QString& ntpServer)
{
    if(!QFile::exists(SETTINGS_FILENAME))
        return false;

    QFile file(SETTINGS_FILENAME);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    QStringList lines = in.readAll().split('\n');
    file.close();

    bool found = false;
    QRegularExpression ntpRegex(R"(^\s*#?\s*NTP\s*=.*$)");

    for (QString &line : lines) {
        if (ntpRegex.match(line).hasMatch()) {
            found = true;
            if (ntpServer.trimmed().isEmpty()) {
                // Comment out the NTP line with no value
                line = "#NTP=";
            } else {
                // Uncomment and set the NTP server(s)
                line = QString("NTP=%1").arg(ntpServer.trimmed());
            }
            break;
        }
    }

    if (!found) {
        // If no existing NTP line, add it under [Time] section
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed() == "[Time]") {
                lines.insert(i + 1, ntpServer.trimmed().isEmpty()
                             ? "#NTP="
                             : QString("NTP=%1").arg(ntpServer.trimmed()));
                found = true;
                break;
            }
        }
    }

    if (!found)
        // If still not found, just append it to the end
        lines << (ntpServer.trimmed().isEmpty()
                      ? "#NTP="
                      : QString("NTP=%1").arg(ntpServer.trimmed()));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    QTextStream out(&file);
    out << lines.join('\n') << '\n';
    file.close();

    return true;
}

QString TimedateComponentDBus::getServerName() const
{
    if(!m_timesyncInterface)
        return QString();

    return m_timesyncInterface->serverName();
}
