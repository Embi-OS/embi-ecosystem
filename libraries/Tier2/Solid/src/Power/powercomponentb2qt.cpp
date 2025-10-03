#include "powercomponentb2qt.h"

#include <QProcess>
#include <unistd.h>
#include <sys/reboot.h>

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

PowerComponentB2qt::PowerComponentB2qt() :
    AbstractPowerComponent()
{

}

int PowerComponentB2qt::getCapabilities()
{
    return Capabilities::Quit |
           Capabilities::Restart |
           Capabilities::Shutdown |
           Capabilities::Reboot |
           Capabilities::Launch;
}

void PowerComponentB2qt::quit()
{
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
}

void PowerComponentB2qt::restart()
{
    QMetaObject::invokeMethod(qApp, [](){
        ::sync();
        qApp->quit();
        QProcess::startDetached(APPCONTROLLER_CMD, QStringList("--restart"));
    }, Qt::QueuedConnection);
}

void PowerComponentB2qt::shutdown()
{
    QMetaObject::invokeMethod(qApp, [](){
        ::sync();
        qApp->quit();
        QProcess::startDetached("shutdown", QStringList("now"));
    }, Qt::QueuedConnection);
}

void PowerComponentB2qt::reboot()
{
    QMetaObject::invokeMethod(qApp, [](){
        ::sync();
        qApp->quit();
        QProcess::startDetached("reboot", {});
    }, Qt::QueuedConnection);
}

void PowerComponentB2qt::launch(const QString& path)
{
    QMetaObject::invokeMethod(qApp, [path](){
        qApp->quit();
        QProcess::startDetached("systemd-run", {APPCONTROLLER_CMD, path});
    }, Qt::QueuedConnection);
}
