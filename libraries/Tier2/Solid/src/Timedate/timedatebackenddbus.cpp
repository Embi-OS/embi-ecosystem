#include "timedatebackenddbus.h"

#include "timedate1interface.h"
#include "timesync1interface.h"

#define SETTINGS_FILENAME "/etc/systemd/timesyncd.conf"

TimedateBackendDbus::TimedateBackendDbus(QObject* parent):
    TimedateBackend(parent),
    m_timedateInterface(new OrgFreedesktopTimedate1Interface(QStringLiteral("org.freedesktop.timedate1"),
                                                             QStringLiteral("/org/freedesktop/timedate1"),
                                                             QDBusConnection::systemBus(), this)),
    m_timesyncInterface(new OrgFreedesktopTimesync1Interface(QStringLiteral("org.freedesktop.timesync1"),
                                                             QStringLiteral("/org/freedesktop/timesync1"),
                                                             QDBusConnection::systemBus(), this))

{

}

int TimedateBackendDbus::getCapabilities()
{
    return Capabilities::Timezone |
           Capabilities::Ntp |
           Capabilities::SystemDateTime |
           Capabilities::NtpServer;
}

QString TimedateBackendDbus::getTimezone() const
{
    return m_timedateInterface->timezone();
}

bool TimedateBackendDbus::setTimezone(const QString &timezone)
{
    do {
        m_timedateInterface->SetTimezone(timezone, true);
    } while (getTimezone() != timezone);

    return true;
}

bool TimedateBackendDbus::getNtp() const
{
    return m_timedateInterface->nTP();
}

bool TimedateBackendDbus::setNtp(bool ntp)
{
    do {
        m_timedateInterface->SetNTP(ntp, true);
    } while (getNtp() != ntp);

    return true;
}

bool TimedateBackendDbus::setSystemTime(const QDateTime &time)
{
    qlonglong usecsSinceEpoch = time.toMSecsSinceEpoch()*1000;
    m_timedateInterface->SetTime(usecsSinceEpoch, false, true);

    return true;
}

QString TimedateBackendDbus::getNtpServer() const
{
    if (!QFile::exists(QString::fromLatin1(SETTINGS_FILENAME)))
        return QString();

    QFile file(QString::fromLatin1(SETTINGS_FILENAME));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QTextStream in(&file);
    static const QRegularExpression ntpRegex(R"(^\s*#?\s*NTP\s*=\s*(.*)\s*$)");
    while (!in.atEnd()) {
        const QString line = in.readLine();
        const QRegularExpressionMatch match = ntpRegex.match(line);
        if (match.hasMatch())
            return match.captured(1).trimmed();
    }
    return QString();
}

bool TimedateBackendDbus::setNtpServer(const QString &ntpServer)
{
    if (!QFile::exists(QString::fromLatin1(SETTINGS_FILENAME)))
        return false;

    QFile file(QString::fromLatin1(SETTINGS_FILENAME));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines = QTextStream(&file).readAll().split('\n');
    file.close();

    bool found = false;
    const QRegularExpression ntpRegex(R"(^\s*#?\s*NTP\s*=.*$)");
    for (QString &line : lines) {
        if (!ntpRegex.match(line).hasMatch())
            continue;

        found = true;
        line = ntpServer.trimmed().isEmpty()
                   ? QStringLiteral("#NTP=")
                   : QStringLiteral("NTP=%1").arg(ntpServer.trimmed());
        break;
    }

    if (!found) {
        for (int i = 0; i < lines.size(); ++i) {
            if (lines.at(i).trimmed() != QLatin1String("[Time]"))
                continue;

            lines.insert(i + 1,
                         ntpServer.trimmed().isEmpty()
                             ? QStringLiteral("#NTP=")
                             : QStringLiteral("NTP=%1").arg(ntpServer.trimmed()));
            found = true;
            break;
        }
    }

    if (!found) {
        lines << (ntpServer.trimmed().isEmpty()
                      ? QStringLiteral("#NTP=")
                      : QStringLiteral("NTP=%1").arg(ntpServer.trimmed()));
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    QTextStream out(&file);
    out << lines.join('\n') << '\n';
    return true;
}

QString TimedateBackendDbus::getServerName() const
{
    return m_timesyncInterface->serverName();
}
