#include "power.h"
#include "solid_log.h"

#if defined(Q_DEVICE_APALIS_IMX8)
#include "powercomponentapalisimx8.h"
#elif defined(Q_OS_BOOT2QT)
#include "powercomponentb2qt.h"
#elif defined(LINUX_DBUS) || defined(Q_OS_LINUX)
#include "powercomponentlinux.h"
#elif defined(Q_OS_WIN32) || defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)
#endif

#include "abstractpowercomponent.h"

static AbstractPowerComponent* getComponent()
{
    static AbstractPowerComponent* instance = nullptr;

    if(!instance)
    {
#if defined(Q_DEVICE_APALIS_IMX8)
        instance = new PowerComponentApalisIMX8();
#elif defined(Q_OS_BOOT2QT)
        instance = new PowerComponentB2qt();
#elif defined(LINUX_DBUS) || defined(Q_OS_LINUX)
        instance = new PowerComponentLinux();
#elif defined(Q_OS_WIN32) || defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)
#endif
    }

    return instance;
}

Power::Power(QObject *parent) :
    QObject(parent)
{
    if(!getComponent()) {
        SOLIDLOG_WARNING()<<"Could not find a power component matching this platform";
    }
}

bool Power::canQuit()
{
    if(!getComponent())
        return false;
    return getComponent()->canQuit();
}

bool Power::canRestart()
{
    if(!getComponent())
        return false;
    return getComponent()->canRestart();
}

bool Power::canShutdown()
{
    if(!getComponent())
        return false;
    return getComponent()->canShutdown();
}

bool Power::canReboot()
{
    if(!getComponent())
        return false;
    return getComponent()->canReboot();
}

bool Power::canLaunch()
{
    if(!getComponent())
        return false;
    return getComponent()->canLaunch();
}

bool Power::canSuspend()
{
    if(!getComponent())
        return false;
    return getComponent()->canSuspend();
}

bool Power::canWake()
{
    if(!getComponent())
        return false;
    return getComponent()->canWake();
}

bool Power::canAlwaysOn()
{
    if(!getComponent())
        return false;
    return getComponent()->canAlwaysOn();
}

void Power::quit()
{
    if(!canQuit())
    {
        SOLIDLOG_WARNING()<<"Cannot quit";
        return;
    }

    emit Get()->aboutToQuit();

    getComponent()->quit();
    SOLIDLOG_INFO()<<"Quit app...";
}

void Power::restart()
{
    if(!canRestart())
    {
        SOLIDLOG_WARNING()<<"Cannot restart";
        return;
    }

    emit Get()->aboutToRestart();

    getComponent()->restart();
    SOLIDLOG_INFO()<<"Restart app..."<<qApp->applicationFilePath();
}

void Power::shutdown()
{
    if(!canShutdown())
    {
        SOLIDLOG_WARNING()<<"Cannot shutdown";
        return;
    }

    emit Get()->aboutToShutdown();

    getComponent()->shutdown();
    SOLIDLOG_INFO()<<"Shutdown device...";
}

void Power::reboot()
{
    if(!canReboot())
    {
        SOLIDLOG_WARNING()<<"Cannot reboot";
        return;
    }

    emit Get()->aboutToReboot();

    getComponent()->reboot();
    SOLIDLOG_INFO()<<"Reboot device...";
}

void Power::launch(const QString& path)
{
    if(!canLaunch())
    {
        SOLIDLOG_WARNING()<<"Cannot launch";
        return;
    }

    emit Get()->aboutToLaunch(path);

    getComponent()->launch(path);
    SOLIDLOG_INFO()<<"Launch app"<<path;
}

void Power::suspend(bool deep)
{
    if(!canSuspend())
    {
        SOLIDLOG_WARNING()<<"Cannot suspend";
        return;
    }

    emit Get()->aboutToSuspend(deep);

    getComponent()->suspend(deep);
    SOLIDLOG_INFO()<<"Suspend device..."<<deep;
}

void Power::wakeIn(int second)
{
    if(!canWake())
    {
        SOLIDLOG_WARNING()<<"Cannot wake";
        return;
    }

    getComponent()->wakeIn(second);
    SOLIDLOG_INFO()<<"Wake device in"<<second;
}

bool Power::isAlwaysOn()
{
    if(!canLaunch())
    {
        return false;
    }

    return getComponent()->isAlwaysOn();
}
