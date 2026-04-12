#ifndef POWER_H
#define POWER_H

#include <QDefs>
#include "qsingleton.h"
#include "powerbackend.h"

class Power : public QObject,
              public QQmlSingleton<Power>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_PTR_PROPERTY(PowerBackend, backend)

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
    bool canQuit() const;
    bool canRestart() const;
    bool canShutdown() const;
    bool canReboot() const;
    bool canSuspend() const;
    bool canWake() const;
    bool canAlwaysOn() const;

public slots:
    void quit();
    void restart();
    void shutdown();
    void reboot();
    void suspend(bool deep=false);
    void wakeIn(int second);
    bool isAlwaysOn();

signals:
    void aboutToQuit();
    void aboutToRestart();
    void aboutToShutdown();
    void aboutToReboot();
    void aboutToSuspend(bool deep);
};

#endif // POWER_H
