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
    Q_PROPERTY(bool canLaunch   READ canLaunch      CONSTANT FINAL)
    Q_PROPERTY(bool canSuspend  READ canSuspend     CONSTANT FINAL)
    Q_PROPERTY(bool canWake     READ canWake        CONSTANT FINAL)
    Q_PROPERTY(bool canAlwaysOn READ canAlwaysOn    CONSTANT FINAL)

protected:
    friend QQmlSingleton<Power>;
    explicit Power(QObject *parent = nullptr);

public:
    static bool canQuit();
    static bool canRestart();
    static bool canShutdown();
    static bool canReboot();
    static bool canLaunch();
    static bool canSuspend();
    static bool canWake();
    static bool canAlwaysOn();

public slots:
    static void quit();
    static void restart();
    static void shutdown();
    static void reboot();
    static void launch(const QString& path);
    static void suspend(bool deep=false);
    static void wakeIn(int second);
    static bool isAlwaysOn();

signals:
    void aboutToQuit();
    void aboutToRestart();
    void aboutToShutdown();
    void aboutToReboot();
    void aboutToLaunch(const QString& path);
    void aboutToSuspend(bool deep);
};

#endif // POWER_H
