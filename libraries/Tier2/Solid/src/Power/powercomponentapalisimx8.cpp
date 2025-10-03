#include "powercomponentapalisimx8.h"

#include <QProcess>
#include <unistd.h>
#include <sys/reboot.h>

PowerComponentApalisIMX8::PowerComponentApalisIMX8() :
    PowerComponentB2qt()
{

}

int PowerComponentApalisIMX8::getCapabilities()
{
    return PowerComponentB2qt::getCapabilities() |
           Capabilities::Suspend |
           Capabilities::Wake |
           Capabilities::AlwaysOn;
}

void PowerComponentApalisIMX8::suspend(bool deep)
{
    ::sync();
    const QString bash = QString("modprobe -r mwifiex_pcie; echo %1 > /sys/power/mem_sleep; echo mem > /sys/power/state").arg(deep ? "deep" : "s2idle");
    QProcess::startDetached("bash", QStringList()<<"-c"<<bash);
}

void PowerComponentApalisIMX8::wakeIn(int second)
{
    ::sync();
    const QString bash = QString("echo %1 > /sys/class/rtc/rtc1/wakealarm").arg(second);
    QProcess::startDetached("bash", QStringList()<<"-c"<<bash);
}

bool PowerComponentApalisIMX8::isAlwaysOn()
{
    return false;
}
