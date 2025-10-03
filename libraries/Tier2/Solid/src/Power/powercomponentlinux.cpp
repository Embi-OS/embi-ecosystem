#include "powercomponentlinux.h"

#include <QProcess>
#include <unistd.h>

PowerComponentLinux::PowerComponentLinux() :
    AbstractPowerComponent()
{

}

int PowerComponentLinux::getCapabilities()
{
    return Capabilities::Quit |
           Capabilities::Restart;
}

void PowerComponentLinux::quit()
{
    QMetaObject::invokeMethod(qApp, &QCoreApplication::quit, Qt::QueuedConnection);
}

void PowerComponentLinux::restart()
{
    QMetaObject::invokeMethod(qApp, [](){
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }, Qt::QueuedConnection);
}
