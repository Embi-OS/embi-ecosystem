#include "powerbackenddesktop.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QProcess>

PowerBackendDesktop::PowerBackendDesktop(QObject* parent) :
    PowerBackend(parent)
{

}

int PowerBackendDesktop::getCapabilities()
{
    return Capabilities::Quit |
           Capabilities::Restart;
}

void PowerBackendDesktop::quit()
{
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
    }, Qt::QueuedConnection);
}

void PowerBackendDesktop::restart()
{
    QMetaObject::invokeMethod(qApp, []() {
        const QString applicationPath = QCoreApplication::applicationFilePath();
        if (applicationPath.isEmpty())
            return;

        qApp->quit();
        QProcess::startDetached(applicationPath, QCoreApplication::arguments().mid(1));
    }, Qt::QueuedConnection);
}
