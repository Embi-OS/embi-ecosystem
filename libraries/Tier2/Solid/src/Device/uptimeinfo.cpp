#include "uptimeinfo.h"
#include "solid_log.h"

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

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
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
        SOLIDLOG_WARNING()<<"Corrupted uptime - Unexpected tokens count";
        return false;
    }

    m_systemTime = lineSplit.at(0).toDouble();
    m_idleTime = lineSplit.at(1).toDouble();

    long long systemTime = (long long)m_systemTime;
    m_upSeconds = systemTime % 60;
    m_upMinutes = systemTime / 60 % 60;
    m_upHours = systemTime / 3600 % 24;
    m_upDays = systemTime / 86400;


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

    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool UptimeInfo::refresh()
{
    SOLIDLOG_WARNING()<<"UptimeInfo: unsupported platform!";
    return false;
}

#endif
