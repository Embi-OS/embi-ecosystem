#ifndef CLOCK_H
#define CLOCK_H

#include <Axion>
class Clock : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(Clock)

    Q_COMPOSITION_PROPERTY(AlarmModel, alarmModel, nullptr)

    Q_READONLY_VAR_PROPERTY(bool, ringing, Ringing, false)
    Q_READONLY_VAR_PROPERTY(bool, snoozed, Snoozed, false)
    Q_WRITABLE_VAR_PROPERTY(int, ringTimeout, RingTimeout, 30)
    Q_WRITABLE_VAR_PROPERTY(int, wakeTimeout, WakeTimeout, 5)
    Q_WRITABLE_VAR_PROPERTY(int, snoozeTimeout, SnoozeTimeout, 5)
    Q_WRITABLE_VAR_PROPERTY(int, defaultVolume, DefaultVolume, 20)

private:
    explicit Clock(QObject *parent = nullptr);

public:
    bool init() final override;
    void postInit() final override;

public slots:
    void snooze();
    void stopAlarm();
    void startAlarm();

signals:
    void startRinging();
    void stopRinging();

private:
    QTimer* m_ringTimer=nullptr;
    QTimer* m_snoozeTimer=nullptr;
};

#endif // CLOCK_H
