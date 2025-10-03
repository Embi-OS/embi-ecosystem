#ifndef ABSTRACTPOWERCOMPONENT_H
#define ABSTRACTPOWERCOMPONENT_H

#include <QCoreApplication>

class AbstractPowerComponent
{
public:
    AbstractPowerComponent() {};

    enum Capabilities
    {
        Quit        = ( 1<<0 ),
        Restart     = ( 1<<1 ),
        Shutdown    = ( 1<<2 ),
        Reboot      = ( 1<<3 ),
        Launch      = ( 1<<4 ),
        Suspend     = ( 1<<5 ),
        Wake        = ( 1<<6 ),
        AlwaysOn    = ( 1<<7 ),
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canQuit() { return hasCapability(Capabilities::Quit); }
    bool canRestart() { return hasCapability(Capabilities::Restart); }
    bool canShutdown() { return hasCapability(Capabilities::Shutdown); }
    bool canReboot() { return hasCapability(Capabilities::Reboot); }
    bool canLaunch() { return hasCapability(Capabilities::Launch); }
    bool canSuspend() { return hasCapability(Capabilities::Suspend); }
    bool canWake() { return hasCapability(Capabilities::Wake); }
    bool canAlwaysOn() { return hasCapability(Capabilities::AlwaysOn); }

    virtual int getCapabilities() = 0;

    virtual void quit() {};
    virtual void restart() {};
    virtual void shutdown() {};
    virtual void reboot() {};
    virtual void launch(const QString&) {};
    virtual void suspend(bool deep=false) {};
    virtual void wakeIn(int second) {};
    virtual bool isAlwaysOn() { return false; };
};

#endif // ABSTRACTPOWERCOMPONENT_H
