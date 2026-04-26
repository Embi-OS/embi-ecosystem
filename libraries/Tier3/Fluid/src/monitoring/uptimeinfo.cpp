#include "uptimeinfo.h"
#include "fluid_log.h"

#if defined(Q_OS_WINDOWS)
#include <Windows.h>
#endif

UptimeInfo::UptimeInfo(QObject *parent) :
    QObject(parent)
{
    refresh();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &UptimeInfo::refresh);
    timer->setSingleShot(false);

    connect(this, &UptimeInfo::tickChanged, this, [timer](int tick) {
        timer->stop();
        if(tick<=0)
            return;
        timer->setInterval(1000);
        timer->start();
    });
}

double UptimeInfo::systemTime() const
{
    return m_systemTime;
}

double UptimeInfo::idleTime() const
{
    return m_idleTime;
}

int UptimeInfo::upSeconds() const
{
    return m_upSeconds;
}

int UptimeInfo::upMinutes() const
{
    return m_upMinutes;
}

int UptimeInfo::upHours() const
{
    return m_upHours;
}

int UptimeInfo::upDays() const
{
    return m_upDays;
}

const QString& UptimeInfo::upTime() const
{
    return m_upTime;
}

void UptimeInfo::updateUptimeFields()
{
    long long seconds = static_cast<long long>(m_systemTime);
    m_upSeconds = seconds % 60;
    m_upMinutes = seconds / 60 % 60;
    m_upHours = seconds / 3600 % 24;
    m_upDays = seconds / 86400;

    if (m_upDays <= 0 && m_upHours <= 0 && m_upMinutes <= 0)
    {
        m_upTime = tr("Moins d'une minute");
    }
    else
    {
        QString daysFormatted = tr("%1 jour(s) et ").arg(m_upDays);
        QTime time(m_upHours, m_upMinutes, m_upSeconds);
        m_upTime = QString("%1%2").arg(daysFormatted,time.toString());
    }
}

#if defined(Q_OS_WINDOWS)
/*****************************************************************************
 * Windows
 ****************************************************************************/

quint64 fileTimeToHundredNanoseconds(const FILETIME& fileTime)
{
    ULARGE_INTEGER value;
    value.LowPart = fileTime.dwLowDateTime;
    value.HighPart = fileTime.dwHighDateTime;
    return value.QuadPart;
}

bool UptimeInfo::refresh()
{
    m_systemTime = static_cast<double>(GetTickCount64()) / 1000.0;

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if(GetSystemTimes(&idleTime, &kernelTime, &userTime))
        m_idleTime = static_cast<double>(fileTimeToHundredNanoseconds(idleTime)) / 10000000.0;
    else
        m_idleTime = 0;

    updateUptimeFields();

    emit updated();

    return true;
}

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
/*****************************************************************************
 * GNU/Linux
 ****************************************************************************/

bool UptimeInfo::refresh()
{
    QFile file(QStringLiteral("/proc/uptime"));
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QString uptime = file.readAll().trimmed();
    file.close();

    QStringList lineSplit = uptime.split(" ");
    lineSplit.removeAll("");

    if(lineSplit.size()!=2)
    {
        FLUIDLOG_WARNING()<<"Corrupted uptime - Unexpected tokens count";
        return false;
    }

    m_systemTime = lineSplit.at(0).toDouble();
    m_idleTime = lineSplit.at(1).toDouble();

    updateUptimeFields();

    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool UptimeInfo::refresh()
{
    m_systemTime = 0;
    m_idleTime = 0;
    m_upSeconds = 0;
    m_upMinutes = 0;
    m_upHours = 0;
    m_upDays = 0;
    m_upTime.clear();
    emit updated();
    return false;
}

#endif
