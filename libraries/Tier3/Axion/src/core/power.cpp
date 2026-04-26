#include "power.h"
#include "axion_log.h"

#include <QProcess>

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

Power::Power(QObject *parent) :
    QObject(parent)
{

}

int Power::getCapabilities()
{
#if defined(Q_DEVICE_APALIS_IMX8)
    return Capabilities::Quit |
           Capabilities::Restart |
           Capabilities::Shutdown |
           Capabilities::Reboot |
           Capabilities::Suspend |
           Capabilities::Wake |
           Capabilities::AlwaysOn;
#elif defined(Q_OS_BOOT2QT)
    return Capabilities::Quit |
           Capabilities::Restart |
           Capabilities::Shutdown |
           Capabilities::Reboot;
#elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))
    return Capabilities::Quit |
           Capabilities::Restart;
#else
    return Capabilities::Quit;
#endif
}

void Power::quit()
{
    if(!canQuit())
    {
        AXIONLOG_WARNING()<<"Cannot quit";
        return;
    }

    emit Get()->aboutToQuit();

    AXIONLOG_INFO()<<"Quit app...";

    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
    }, Qt::QueuedConnection);
}

void Power::restart()
{
    if(!canRestart())
    {
        AXIONLOG_WARNING()<<"Cannot restart";
        return;
    }

    emit Get()->aboutToRestart();

    AXIONLOG_INFO()<<"Restart app..."<<qApp->applicationFilePath();

#if defined(Q_OS_BOOT2QT)
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral(APPCONTROLLER_CMD), {QStringLiteral("--restart")});
    }, Qt::QueuedConnection);
#elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))
    QMetaObject::invokeMethod(qApp, []() {
        const QString applicationPath = QCoreApplication::applicationFilePath();
        if (applicationPath.isEmpty())
            return;

        qApp->quit();
        QProcess::startDetached(applicationPath, QCoreApplication::arguments().mid(1));
    }, Qt::QueuedConnection);
#endif
}

void Power::shutdown()
{
    if(!canShutdown())
    {
        AXIONLOG_WARNING()<<"Cannot shutdown";
        return;
    }

    emit Get()->aboutToShutdown();

    AXIONLOG_INFO()<<"Shutdown device...";

#if defined(Q_OS_BOOT2QT)
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral("shutdown"), {QStringLiteral("now")});
    }, Qt::QueuedConnection);
#endif
}

void Power::reboot()
{
    if(!canReboot())
    {
        AXIONLOG_WARNING()<<"Cannot reboot";
        return;
    }

    emit Get()->aboutToReboot();

    AXIONLOG_INFO()<<"Reboot device...";

#if defined(Q_OS_BOOT2QT)
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral("reboot"), {});
    }, Qt::QueuedConnection);
#endif
}

void Power::suspend(bool deep)
{
    if(!canSuspend())
    {
        AXIONLOG_WARNING()<<"Cannot suspend";
        return;
    }

    emit Get()->aboutToSuspend(deep);

    AXIONLOG_INFO()<<"Suspend device..."<<deep;

#if defined(Q_DEVICE_APALIS_IMX8)
    QMetaObject::invokeMethod(qApp, [deep]() {
        const QString bash = QStringLiteral("modprobe -r mwifiex_pcie; echo %1 > /sys/power/mem_sleep; echo mem > /sys/power/state")
        .arg(deep ? QStringLiteral("deep") : QStringLiteral("s2idle"));
        QProcess::startDetached(QStringLiteral("bash"), {QStringLiteral("-c"), bash});
    }, Qt::QueuedConnection);
#endif
}

void Power::wakeIn(int second)
{
    if(!canWake())
    {
        AXIONLOG_WARNING()<<"Cannot wake";
        return;
    }

    AXIONLOG_INFO()<<"Wake device in"<<second;

#if defined(Q_DEVICE_APALIS_IMX8)
    QMetaObject::invokeMethod(qApp, [second]() {
        const QString bash = QStringLiteral("echo %1 > /sys/class/rtc/rtc1/wakealarm").arg(second);
        QProcess::startDetached(QStringLiteral("bash"), {QStringLiteral("-c"), bash});
    }, Qt::QueuedConnection);
#endif
}

bool Power::isAlwaysOn()
{
    if(!canAlwaysOn())
        return false;

    return false;
}
