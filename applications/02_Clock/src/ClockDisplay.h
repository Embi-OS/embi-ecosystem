#ifndef CLOCKDISPLAY_H
#define CLOCKDISPLAY_H

#include <Axion>

#include <QPropertyAnimation>

Q_ENUM_CLASS(ClockDisplayStates, ClockDisplayState,
             Invalid,
             On,
             Off,
             Locking,
             Waking,
             AlwaysOn,
             Locked)

class ClockDisplay : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(ClockDisplay)


    Q_READONLY_VAR_PROPERTY(ClockDisplayStates::Enum, state, State, ClockDisplayStates::Invalid)

    Q_WRITABLE_VAR_PROPERTY(int, onBrightness, OnBrightness, 100)
    Q_WRITABLE_VAR_PROPERTY(int, lockedBrightness, LockedBrightness, 25)
    Q_WRITABLE_VAR_PROPERTY(int, lockedTimeout, LockedTimeout, 5)
    Q_WRITABLE_VAR_PROPERTY(int, wakeBrightness, WakeBrightness, 255)
    Q_WRITABLE_VAR_PROPERTY(int, wakeTimeout, WakeTimeout, 10)

    Q_WRITABLE_VAR_PROPERTY(bool, alwaysOnEnabled, AlwaysOnEnabled, false)

private:
    explicit ClockDisplay(QObject *parent = nullptr);

public:
    bool init() final override;

public slots:
    void gotoSleep();
    void gotoAlwaysOn();
    void gotoLocked();
    void gotoOff();
    void gotoWaking();
    void unlock();

private slots:
    void onStateChanged();
    void onTimerTimeout();

private:
    QPropertyAnimation* m_brightnessAnimation=nullptr;
    QTimer* m_timer=nullptr;
};

#endif // CLOCKDISPLAY_H
