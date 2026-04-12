#include "powerbackendb2qt.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QProcess>

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

PowerBackendB2Qt::PowerBackendB2Qt(QObject* parent) :
    PowerBackend(parent)
{

}

int PowerBackendB2Qt::getCapabilities()
{
    return Capabilities::Quit |
           Capabilities::Restart |
           Capabilities::Shutdown |
           Capabilities::Reboot;
}

void PowerBackendB2Qt::quit()
{
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
    }, Qt::QueuedConnection);
}

void PowerBackendB2Qt::restart()
{
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral(APPCONTROLLER_CMD), {QStringLiteral("--restart")});
    }, Qt::QueuedConnection);
}

void PowerBackendB2Qt::shutdown()
{
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral("shutdown"), {QStringLiteral("now")});
    }, Qt::QueuedConnection);
}

void PowerBackendB2Qt::reboot()
{
    QMetaObject::invokeMethod(qApp, []() {
        qApp->quit();
        QProcess::startDetached(QStringLiteral("reboot"), {});
    }, Qt::QueuedConnection);
}
