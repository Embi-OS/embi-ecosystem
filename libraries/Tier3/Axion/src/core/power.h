#ifndef POWER_H
#define POWER_H

#include <QDefs>
#include "qsingleton.h"

class Power : public QObject,
              public QQmlSingleton<Power>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canQuit     READ canQuit        CONSTANT FINAL)
    Q_PROPERTY(bool canRestart  READ canRestart     CONSTANT FINAL)
    Q_PROPERTY(bool canShutdown READ canShutdown    CONSTANT FINAL)
    Q_PROPERTY(bool canReboot   READ canReboot      CONSTANT FINAL)
    Q_PROPERTY(bool canSuspend  READ canSuspend     CONSTANT FINAL)
    Q_PROPERTY(bool canWake     READ canWake        CONSTANT FINAL)
    Q_PROPERTY(bool canAlwaysOn READ canAlwaysOn    CONSTANT FINAL)

protected:
    friend QQmlSingleton<Power>;
    explicit Power(QObject *parent = nullptr);

public:
    enum Capabilities
    {
        Quit        = ( 1<<0 ),
        Restart     = ( 1<<1 ),
        Shutdown    = ( 1<<2 ),
        Reboot      = ( 1<<3 ),
        Suspend     = ( 1<<4 ),
        Wake        = ( 1<<5 ),
        AlwaysOn    = ( 1<<6 ),
    };
    Q_ENUM(Capabilities);

    static int getCapabilities();
    static bool hasCapability(int capability) { return getCapabilities() & capability; };
    static bool canQuit() { return hasCapability(Capabilities::Quit); }
    static bool canRestart() { return hasCapability(Capabilities::Restart); }
    static bool canShutdown() { return hasCapability(Capabilities::Shutdown); }
    static bool canReboot() { return hasCapability(Capabilities::Reboot); }
    static bool canSuspend() { return hasCapability(Capabilities::Suspend); }
    static bool canWake() { return hasCapability(Capabilities::Wake); }
    static bool canAlwaysOn() { return hasCapability(Capabilities::AlwaysOn); }

public slots:
    static void quit();
    static void restart();
    static void shutdown();
    static void reboot();
    static void suspend(bool deep=false);
    static void wakeIn(int second);
    static bool isAlwaysOn();

signals:
    void aboutToQuit();
    void aboutToRestart();
    void aboutToShutdown();
    void aboutToReboot();
    void aboutToSuspend(bool deep);
};

#endif // POWER_H
