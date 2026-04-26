#include "timedatesettings.h"
#include "solid_log.h"

#include <QStandardPaths>
#include <QProcess>

#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
    #include "timedate1interface.h"
    #include "timesync1interface.h"
#endif

#define SETTINGS_FILENAME "/etc/systemd/timesyncd.conf"

TimedateSettings::TimedateSettings(QObject *parent) :
    QObject(parent),
#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
    m_timedateInterface(new OrgFreedesktopTimedate1Interface(QStringLiteral("org.freedesktop.timedate1"),
                                                             QStringLiteral("/org/freedesktop/timedate1"),
                                                             QDBusConnection::systemBus(), this)),
    m_timesyncInterface(new OrgFreedesktopTimesync1Interface(QStringLiteral("org.freedesktop.timesync1"),
                                                             QStringLiteral("/org/freedesktop/timesync1"),
                                                             QDBusConnection::systemBus(), this)),
#endif
    m_timer(new QTimer(this)),
    m_systemDateTimeCaller(new QTimer(this))
{
    if (!m_timedateInterface) {
        SOLIDLOG_WARNING() << "Could not find a timedate backend matching this platform";
    }
    if (!m_timesyncInterface) {
        SOLIDLOG_WARNING() << "Could not find a timesync backend matching this platform";
    }

    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &TimedateSettings::refreshDateTime);

    m_systemDateTimeCaller->setSingleShot(true);
    connect(m_systemDateTimeCaller, &QTimer::timeout, this, &TimedateSettings::updateSystemDateTime);
}

int TimedateSettings::getCapabilities() const
{
    if (!m_timedateInterface)
        return 0;

    return Capabilities::Timezone |
           Capabilities::Ntp |
           Capabilities::SystemDateTime |
           Capabilities::NtpServer;
}

bool TimedateSettings::canSyncRTC() const
{
    static bool available = !QStandardPaths::findExecutable("hwclock").isEmpty();
    return available;
}

QString TimedateSettings::getTimezone() const
{
    if(!m_timedateInterface)
    {
        SOLIDLOG_DEBUG()<<"Cannot get timezone, fallback to default";
        return QTimeZone::systemTimeZoneId();
    }

    return m_timedateInterface->timezone();
}

bool TimedateSettings::setTimezone(const QString& timezone)
{
    if(!m_timedateInterface || !canSetTimezone())
    {
        SOLIDLOG_WARNING()<<"Cannot set timezone";
        return false;
    }

    do {
        m_timedateInterface->SetTimezone(timezone, true);
    } while (getTimezone() != timezone);

    emit this->timezoneChanged();

    return true;
}

bool TimedateSettings::getNtp() const
{
    if(!m_timedateInterface)
    {
        SOLIDLOG_DEBUG()<<"Cannot get ntp, fallback to default";
        return false;
    }

    return m_timedateInterface->nTP();
}

bool TimedateSettings::setNtp(bool ntp)
{
    if(!m_timedateInterface || !canSetNtp())
    {
        SOLIDLOG_WARNING()<<"Cannot set ntp";
        return false;
    }

    do {
        m_timedateInterface->SetNTP(ntp, true);
    } while (getNtp() != ntp);

    emit this->ntpChanged();

    return true;
}

QDateTime TimedateSettings::getSystemDateTime() const
{
    return QDateTime::currentDateTime();
}

QDate TimedateSettings::getSystemDate() const
{
    return getSystemDateTime().date();
}

QTime TimedateSettings::getSystemTime() const
{
    return getSystemDateTime().time();
}

QString TimedateSettings::getNtpServer() const
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

bool TimedateSettings::setNtpServer(const QString& ntpServer)
{
    if(!canSetNtpServer())
    {
        SOLIDLOG_WARNING()<<"Cannot set ntpServer";
        return false;
    }

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

    emit this->ntpServerChanged();

    return true;
}

QString TimedateSettings::getServerName() const
{
    if(!m_timesyncInterface)
    {
        SOLIDLOG_DEBUG()<<"Cannot get server name, fallback to default";
        return QString();
    }

    return m_timesyncInterface->serverName();
}

bool TimedateSettings::setSystemDateTime(const QDateTime& systemDateTime)
{
    if(!m_timedateInterface || !canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system dateTime";
        return false;
    }

    qlonglong usecsSinceEpoch = systemDateTime.toMSecsSinceEpoch()*1000;
    m_timedateInterface->SetTime(usecsSinceEpoch, false, true);

    refreshDateTime();

    return true;
}

bool TimedateSettings::setSystemDate(const QDate& systemDate)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system date";
        return false;
    }

    m_systemDateTemp = systemDate;
    m_systemDateTimeCaller->start();

    return true;
}

bool TimedateSettings::setSystemTime(const QTime& systemTime)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system time";
        return false;
    }

    m_systemTimeTemp = systemTime;
    m_systemDateTimeCaller->start();

    return true;
}

QString TimedateSettings::hwclock(const QStringList& arguments, bool* ok)
{
    bool result = false;
    const QString program = "hwclock";

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    result = process.waitForFinished(5000);

    const QString processOutput = process.readAllStandardOutput();
    const QString processError = process.readAllStandardError();

    if(!result)
    {
        if(!processOutput.isEmpty()) {
            SOLIDLOG_CRITICAL().noquote()<<QString("processOutput:\n%1").arg(processOutput);
        }
        if(!processError.isEmpty()) {
            SOLIDLOG_CRITICAL().noquote()<<QString("processError:\n%1").arg(processError);
        }
    }
    else
    {
        SOLIDLOG_DEBUG().noquote()<<QString("processOutput:\n%1").arg(processOutput);
    }

    if(ok)
        *ok = result;

    if(!processError.isEmpty())
        return processError;

    return processOutput;
}

bool TimedateSettings::syncRtc()
{
    bool result = true;
    if(canSyncRTC())
    {
        const QStringList arguments = getNtp() ? QStringList()<<"-w" : QStringList()<<"-s";
        hwclock(arguments, &result);

        if(result) {
            SOLIDLOG_DEBUG()<<"Successfully synced the system time with the RTC";
        }
    }

    return result;
}

void TimedateSettings::refreshDateTime()
{
    m_timer->stop();

    const QDateTime datetime = QDateTime::currentDateTime();

    emit this->systemDateTimeChanged();

    // TimeDate will be refresh every second at 500msec
    int timer = 1000 + (500-datetime.time().msec());
    m_timer->start(timer);
}

void TimedateSettings::updateSystemDateTime()
{
    if(m_systemDateTemp.isNull() || !m_systemDateTemp.isValid())
        m_systemDateTemp = getSystemDate();
    if(m_systemTimeTemp.isNull() || !m_systemTimeTemp.isValid())
        m_systemTimeTemp = getSystemTime();

    setSystemDateTime(QDateTime(m_systemDateTemp, m_systemTimeTemp));

    m_systemDateTemp = QDate();
    m_systemTimeTemp = QTime();
}
