#include "timedatesettings.h"
#include "timedatebackend.h"
#include "solid_log.h"

#include <QProcess>

#include "timedatebackenddbus.h"

TimedateSettings::TimedateSettings(QObject *parent) :
    QObject(parent),
#if defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX)
    m_backend(new TimedateBackendDbus(this)),
#else
    m_backend(nullptr),
#endif
    m_timer(new QTimer(this)),
    m_systemDateTimeCaller(new QTimer(this))
{
    if (!m_backend) {
        SOLIDLOG_WARNING() << "Could not find a timedate backend matching this platform";
    }

    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &TimedateSettings::refreshDateTime);

    m_systemDateTimeCaller->setSingleShot(true);
    connect(m_systemDateTimeCaller, &QTimer::timeout, this, &TimedateSettings::updateSystemDateTime);

    QTimer* timerSync = new QTimer(this);
    timerSync->setSingleShot(false);
    timerSync->setInterval(3600000);
    timerSync->start();
    connect(timerSync, &QTimer::timeout, this, &TimedateSettings::syncRtc);

    syncRtc();
}

bool TimedateSettings::canSetTimezone() const
{
    return m_backend && m_backend->canSetTimezone();
}

bool TimedateSettings::canSetNtp() const
{
    return m_backend && m_backend->canSetNtp();
}

bool TimedateSettings::canSetSystemDateTime() const
{
    return m_backend && m_backend->canSetSystemDateTime();
}

bool TimedateSettings::canSetNtpServer() const
{
    return m_backend && m_backend->canSetNtpServer();
}

bool TimedateSettings::canReadRTC() const
{
#ifdef Q_OS_BOOT2QT
    return true;
#else
    return false;
#endif
}

QString TimedateSettings::getTimezone() const
{
    if(!m_backend)
    {
        SOLIDLOG_DEBUG()<<"Cannot get timezone, fallback to default";
        return QTimeZone::systemTimeZoneId();
    }

    return m_backend->getTimezone();
}

void TimedateSettings::setTimezone(const QString& timezone)
{
    if(!canSetTimezone())
    {
        SOLIDLOG_WARNING()<<"Cannot set timezone";
        return;
    }

    if(m_backend->setTimezone(timezone))
        emit this->timezoneChanged();
}

bool TimedateSettings::getNtp() const
{
    if(!m_backend)
    {
        SOLIDLOG_DEBUG()<<"Cannot get ntp, fallback to default";
        return false;
    }

    return m_backend->getNtp();
}

void TimedateSettings::setNtp(const bool ntp)
{
    if(!canSetNtp())
    {
        SOLIDLOG_WARNING()<<"Cannot set ntp";
        return;
    }

    if(m_backend->setNtp(ntp))
        emit this->ntpChanged();
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
    if(!m_backend)
    {
        SOLIDLOG_DEBUG()<<"Cannot get ntp, fallback to default";
        return QString();
    }

    return m_backend->getNtpServer();
}

void TimedateSettings::setNtpServer(const QString& ntpServer)
{
    if(!canSetNtpServer())
    {
        SOLIDLOG_WARNING()<<"Cannot set ntpServer";
        return;
    }

    if(m_backend->setNtpServer(ntpServer))
        emit this->ntpServerChanged();

    syncNtp();
}

QString TimedateSettings::getServerName() const
{
    if(!m_backend)
    {
        SOLIDLOG_DEBUG()<<"Cannot get server name, fallback to default";
        return QString();
    }

    return m_backend->getServerName();
}

void TimedateSettings::setSystemDateTime(const QDateTime& systemDateTime)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system dateTime";
        return;
    }

    if(m_backend->setSystemTime(systemDateTime))
    {
        refreshDateTime();
    }
}

void TimedateSettings::setSystemDate(const QDate& systemDate)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system date";
        return;
    }

    m_systemDateTemp = systemDate;
    m_systemDateTimeCaller->start();
}

void TimedateSettings::setSystemTime(const QTime& systemTime)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system time";
        return;
    }

    m_systemTimeTemp = systemTime;
    m_systemDateTimeCaller->start();
}

QString TimedateSettings::timedateCtl()
{
    const QString program = "timedatectl";

    QProcess process;
    process.setProgram(program);
    process.start();

    process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();
    QString processError = process.readAllStandardError();

    if(!processError.isEmpty())
        return processError;

    QStringList ret;
    const QStringList processOutputs = processOutput.split("\n", Qt::SkipEmptyParts);
    for(const QString& output: processOutputs)
        ret.append(output.trimmed());

    return ret.join("\n");
}

QString TimedateSettings::timesyncStatus()
{
    const QString program = "timedatectl";
    const QStringList arguments = QStringList()<<"timesync-status";

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();
    QString processError = process.readAllStandardError();

    if(!processError.isEmpty())
        return processError;

    QStringList ret;
    const QStringList processOutputs = processOutput.split("\n", Qt::SkipEmptyParts);
    for(const QString& output: processOutputs)
        ret.append(output.trimmed());

    return ret.join("\n");
}

bool TimedateSettings::syncNtp()
{
    const QString program = "systemctl";
    const QStringList arguments = QStringList()<<"restart"<<"systemd-timesyncd";

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        QString processOutput = proc->readAllStandardOutput();
        QString processError = proc->readAllStandardError();

        if(!processOutput.isEmpty()) {
            SOLIDLOG_TRACE().noquote()<<QString("processOutput:\n%1").arg(processOutput);
        }
        if(!processError.isEmpty()) {
            SOLIDLOG_CRITICAL().noquote()<<QString("processError:\n%1").arg(processError);
        }

        emit this->timesyncChanged();

        proc->deleteLater();
    });
    proc->setProgram(program);
    proc->setArguments(arguments);
    proc->start();

    return proc->waitForStarted();
}

bool TimedateSettings::syncRtc()
{
    QElapsedTimer timer;
    timer.start();

    bool result = true;
    if(canReadRTC() && !getNtp())
    {
        const QString program = "hwclock";
        const QStringList arguments = QStringList()<<"-s";

        QProcess process;
        process.setProgram(program);
        process.setArguments(arguments);
        process.start();

        result = process.waitForFinished(5000);

        QString processOutput = process.readAllStandardOutput();
        QString processError = process.readAllStandardError();

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
            SOLIDLOG_DEBUG()<<"Successfully synced the system time with the RTC in"<<timer.nsecsElapsed()/1000000.0<<"ms";
        }
    }
    if(canReadRTC() && getNtp())
    {
        const QString program = "hwclock";
        const QStringList arguments = QStringList()<<"-w";

        QProcess process;
        process.setProgram(program);
        process.setArguments(arguments);
        process.start();

        result = process.waitForFinished(5000);

        QString processOutput = process.readAllStandardOutput();
        QString processError = process.readAllStandardError();

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
            SOLIDLOG_DEBUG()<<"Successfully synced the system time with the RTC in"<<timer.nsecsElapsed()/1000000.0<<"ms";
        }
    }

    refreshDateTime();

    return result;
}

void TimedateSettings::refreshDateTime()
{
    m_timer->stop();

    QDateTime datetime = QDateTime::currentDateTime();

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
