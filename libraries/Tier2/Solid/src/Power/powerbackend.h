#ifndef POWERBACKEND_H
#define POWERBACKEND_H

#include <QDefs>

class PowerBackend: public QObject
{
public:
    explicit PowerBackend(QObject* parent=nullptr): QObject(parent) {};

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

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canQuit() { return hasCapability(Capabilities::Quit); }
    bool canRestart() { return hasCapability(Capabilities::Restart); }
    bool canShutdown() { return hasCapability(Capabilities::Shutdown); }
    bool canReboot() { return hasCapability(Capabilities::Reboot); }
    bool canSuspend() { return hasCapability(Capabilities::Suspend); }
    bool canWake() { return hasCapability(Capabilities::Wake); }
    bool canAlwaysOn() { return hasCapability(Capabilities::AlwaysOn); }

    virtual int getCapabilities() = 0;

    virtual void quit() {};
    virtual void restart() {};
    virtual void shutdown() {};
    virtual void reboot() {};
    virtual void suspend(bool deep=false) {};
    virtual void wakeIn(int second) {};
    virtual bool isAlwaysOn() { return false; };
};


#endif // POWERBACKEND_H
