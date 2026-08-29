#include "power.h"
#include "axion_log.h"

#include <QProcess>

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

Power::AcionOnQuit Power::s_actionOnQuit = Power::None;
QString Power::s_executable = QString();
QStringList Power::s_args = QStringList();
QString Power::s_workingPath = QString();

Power::Power(QObject *parent) :
    QObject(parent)
{

}

void Power::init()
{
    s_actionOnQuit = None;
    s_executable = QCoreApplication::applicationFilePath();
    s_args = QCoreApplication::arguments().mid(1);
    s_workingPath = QDir::currentPath();
}

void Power::init(int argc, char *argv[])
{
    s_actionOnQuit = None;
    s_executable = QString::fromLocal8Bit(argv[0]);
    s_args.clear();
    for (int i = 1; i < argc; ++i)
        s_args << QString::fromLocal8Bit(argv[i]);
    s_workingPath = QDir::currentPath();
}

void Power::unInit()
{
    if(canRestart() && s_actionOnQuit==RestartOnQuit)
        doRestart();
    else if(canShutdown() && s_actionOnQuit==ShutdownOnQuit)
        doShutdown();
    else if(canReboot() && s_actionOnQuit==RebootOnQuit)
        doReboot();
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

    s_actionOnQuit = None;
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
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

    s_actionOnQuit = RestartOnQuit;
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
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

    s_actionOnQuit = ShutdownOnQuit;
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
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

    s_actionOnQuit = RebootOnQuit;
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
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

void Power::doRestart()
{
#if defined(Q_OS_BOOT2QT)
    QProcess::startDetached(QStringLiteral(APPCONTROLLER_CMD), {QStringLiteral("--restart")});
#elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))
    if (s_executable.isEmpty())
        return;
    QProcess::startDetached(s_executable, s_args, s_workingPath);
#endif
}
void Power::doShutdown()
{
#if defined(Q_OS_BOOT2QT)
    QProcess::startDetached(QStringLiteral("shutdown"), {QStringLiteral("now")});
#endif
}
void Power::doReboot()
{
#if defined(Q_OS_BOOT2QT)
    QProcess::startDetached(QStringLiteral("reboot"), {});
#endif
}