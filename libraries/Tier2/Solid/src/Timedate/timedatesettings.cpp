#include "timedatesettings.h"
#include "solid_log.h"

#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
#include "timedatecomponentdbus.h"
#endif
#include "abstracttimedatecomponent.h"

static AbstractTimedateComponent* getComponent()
{
    static AbstractTimedateComponent* instance = nullptr;

    if(!instance)
    {
#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
        instance = new TimedateComponentDBus();
#endif
    }

    return instance;
}

TimedateSettings::TimedateSettings(QObject *parent) :
    QObject(parent),
    m_timer(new QTimer(this)),
    m_systemDateTimeCaller(new QTimer(this))
{
    if(!getComponent()) {
        SOLIDLOG_WARNING()<<"Could not find a timedate component matching this platform";
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

    sync();
}

bool TimedateSettings::canSetTimezone()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetTimezone();
}

bool TimedateSettings::canSetNtp()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetNtp();
}

bool TimedateSettings::canSetTimeservers()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetTimeservers();
}

bool TimedateSettings::canSetSystemDateTime()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetSystemDateTime();
}

bool TimedateSettings::canReadRTC()
{
#ifdef Q_OS_BOOT2QT
    return true;
#else
    return false;
#endif
}

QString TimedateSettings::getTimezone() const
{
    return QTimeZone::systemTimeZoneId();
}

void TimedateSettings::setTimezone(const QString& timezone)
{
    if(!canSetTimezone())
    {
        SOLIDLOG_WARNING()<<"Cannot set timezone";
        return;
    }

    if(getComponent()->setTimezone(timezone))
        emit this->timezoneChanged();
}

bool TimedateSettings::getNtp() const
{
    if(!canSetNtp())
    {
        SOLIDLOG_DEBUG()<<"Cannot get ntp, fallback to default";
        return false;
    }

    return getComponent()->getNtp();
}

void TimedateSettings::setNtp(const bool ntp)
{
    if(!canSetNtp())
    {
        SOLIDLOG_WARNING()<<"Cannot set ntp";
        return;
    }

    if(getComponent()->setNtp(ntp))
        emit this->ntpChanged();
}

QString TimedateSettings::getTimeservers() const
{
    if(!canSetTimeservers())
    {
        SOLIDLOG_DEBUG()<<"Cannot get timeservers, fallback to default";
        return QString();
    }

    return getComponent()->getTimeservers();
}

void TimedateSettings::setTimeservers(const QString& timeservers)
{
    if(!canSetTimeservers())
    {
        SOLIDLOG_WARNING()<<"Cannot set timeservers";
        return;
    }

    if(getComponent()->setTimeservers(timeservers))
        emit this->timeserversChanged();

    syncNtp();
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

QString TimedateSettings::serverName() const
{
    if(!getComponent())
        return QString();
    return getComponent()->serverName();
}
QString TimedateSettings::serverAddress() const
{
    if(!getComponent())
        return QString();
    return getComponent()->serverAddress();
}
int TimedateSettings::pollIntervalMinUSec() const
{
    if(!getComponent())
        return -1;
    return getComponent()->pollIntervalMinUSec();
}
int TimedateSettings::pollIntervalMaxUSec() const
{
    if(!getComponent())
        return -1;
    return getComponent()->pollIntervalMaxUSec();
}
int TimedateSettings::frequency() const
{
    if(!getComponent())
        return -1;
    return getComponent()->frequency();
}

void TimedateSettings::setSystemDateTime(const QDateTime& systemDateTime)
{
    if(!canSetSystemDateTime())
    {
        SOLIDLOG_WARNING()<<"Cannot set system dateTime";
        return;
    }

    if(getComponent()->setSystemTime(systemDateTime))
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
    QString program="timedatectl";

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

bool TimedateSettings::syncNtp()
{
    QString program="systemctl";
    QStringList arguments = QStringList()<<"restart"<<"systemd-timesyncd";

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    bool result = process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();
    QString processError = process.readAllStandardError();

    if(!processOutput.isEmpty()) {
        SOLIDLOG_TRACE().noquote()<<QString("processOutput:\n%1").arg(processOutput);
    }
    if(!processError.isEmpty()) {
        SOLIDLOG_CRITICAL().noquote()<<QString("processError:\n%1").arg(processError);
    }

    emit this->timesyncChanged();

    return result;
}

bool TimedateSettings::syncRtc()
{
    QElapsedTimer timer;
    timer.start();

    bool result = true;
    if(canReadRTC() && !getNtp())
    {
        QString program="hwclock";
        QStringList arguments = QStringList()<<"-s";

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
        QString program="hwclock";
        QStringList arguments = QStringList()<<"-w";

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
