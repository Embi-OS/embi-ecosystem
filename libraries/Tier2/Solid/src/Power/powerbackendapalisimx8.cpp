#include "powerbackendapalisimx8.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QProcess>

PowerBackendApalisIMX8::PowerBackendApalisIMX8(QObject* parent) :
    PowerBackendB2Qt(parent)
{

}

int PowerBackendApalisIMX8::getCapabilities()
{
    return PowerBackendB2Qt::getCapabilities() |
           Capabilities::Suspend |
           Capabilities::Wake |
           Capabilities::AlwaysOn;
}

void PowerBackendApalisIMX8::suspend(bool deep)
{
    QMetaObject::invokeMethod(qApp, [deep]() {
        const QString bash = QStringLiteral("modprobe -r mwifiex_pcie; echo %1 > /sys/power/mem_sleep; echo mem > /sys/power/state")
                                 .arg(deep ? QStringLiteral("deep") : QStringLiteral("s2idle"));
        QProcess::startDetached(QStringLiteral("bash"), {QStringLiteral("-c"), bash});
    }, Qt::QueuedConnection);
}

void PowerBackendApalisIMX8::wakeIn(int second)
{
    QMetaObject::invokeMethod(qApp, [second]() {
        const QString bash = QStringLiteral("echo %1 > /sys/class/rtc/rtc1/wakealarm").arg(second);
        QProcess::startDetached(QStringLiteral("bash"), {QStringLiteral("-c"), bash});
    }, Qt::QueuedConnection);
}

bool PowerBackendApalisIMX8::isAlwaysOn()
{
    return false;
}
