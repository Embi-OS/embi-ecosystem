#include "power.h"
#include "powerbackend.h"
#include "solid_log.h"

#include "powerbackenddesktop.h"
#include "powerbackendb2qt.h"
#include "powerbackendapalisimx8.h"

Power::Power(QObject *parent) :
    QObject(parent),
#if defined(Q_DEVICE_APALIS_IMX8)
    m_backend(new PowerBackendApalisIMX8(this))
#elif defined(Q_OS_BOOT2QT)
    m_backend(new PowerBackendB2Qt(this))
#elif defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    m_backend(new PowerBackendDesktop(this))
#else
    m_backend(nullptr)
#endif
{
    if(!m_backend) {
        SOLIDLOG_WARNING()<<"Could not find a power component matching this platform";
    }
}

bool Power::canQuit() const
{
    return m_backend && m_backend->canQuit();
}

bool Power::canRestart() const
{
    return m_backend && m_backend->canRestart();
}

bool Power::canShutdown() const
{
    return m_backend && m_backend->canShutdown();
}

bool Power::canReboot() const
{
    return m_backend && m_backend->canReboot();
}

bool Power::canSuspend() const
{
    return m_backend && m_backend->canSuspend();
}

bool Power::canWake() const
{
    return m_backend && m_backend->canWake();
}

bool Power::canAlwaysOn() const
{
    return m_backend && m_backend->canAlwaysOn();
}

void Power::quit()
{
    if(!canQuit())
    {
        SOLIDLOG_WARNING()<<"Cannot quit";
        return;
    }

    emit this->aboutToQuit();

    m_backend->quit();
    SOLIDLOG_INFO()<<"Quit app...";
}

void Power::restart()
{
    if(!canRestart())
    {
        SOLIDLOG_WARNING()<<"Cannot restart";
        return;
    }

    emit this->aboutToRestart();

    m_backend->restart();
    SOLIDLOG_INFO()<<"Restart app..."<<qApp->applicationFilePath();
}

void Power::shutdown()
{
    if(!canShutdown())
    {
        SOLIDLOG_WARNING()<<"Cannot shutdown";
        return;
    }

    emit this->aboutToShutdown();

    m_backend->shutdown();
    SOLIDLOG_INFO()<<"Shutdown device...";
}

void Power::reboot()
{
    if(!canReboot())
    {
        SOLIDLOG_WARNING()<<"Cannot reboot";
        return;
    }

    emit this->aboutToReboot();

    m_backend->reboot();
    SOLIDLOG_INFO()<<"Reboot device...";
}

void Power::suspend(bool deep)
{
    if(!canSuspend())
    {
        SOLIDLOG_WARNING()<<"Cannot suspend";
        return;
    }

    emit this->aboutToSuspend(deep);

    m_backend->suspend(deep);
    SOLIDLOG_INFO()<<"Suspend device..."<<deep;
}

void Power::wakeIn(int second)
{
    if(!canWake())
    {
        SOLIDLOG_WARNING()<<"Cannot wake";
        return;
    }

    m_backend->wakeIn(second);
    SOLIDLOG_INFO()<<"Wake device in"<<second;
}

bool Power::isAlwaysOn()
{
    if(!canAlwaysOn())
        return false;

    return m_backend->isAlwaysOn();
}
