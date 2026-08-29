#ifndef CLOCK_H
#define CLOCK_H

#include <Axion>

#include <QPointer>

#include "helpers/alarmobject.h"
#include "helpers/alarmgroupobject.h"

class Clock : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(Clock)

    Q_CONSTANT_PTR_PROPERTY(AlarmModel, alarmModel)
    Q_CONSTANT_PTR_PROPERTY(AlarmGroupModel, alarmGroupModel)

    Q_READONLY_VAR_PROPERTY(bool, ringing, Ringing, false)
    Q_READONLY_VAR_PROPERTY(bool, snoozed, Snoozed, false)
    Q_READONLY_VAR_PROPERTY(bool, snoozeAvailable, SnoozeAvailable, false)
    Q_WRITABLE_VAR_PROPERTY(int, ringTimeout, RingTimeout, 30)
    Q_WRITABLE_VAR_PROPERTY(int, wakeTimeout, WakeTimeout, 5)
    Q_WRITABLE_VAR_PROPERTY(int, snoozeTimeout, SnoozeTimeout, 5)
    Q_WRITABLE_VAR_PROPERTY(int, defaultVolume, DefaultVolume, 20)

private:
    explicit Clock(QObject *parent = nullptr);

public:
    bool init() final override;
    bool postInit() final override;

public slots:
    void snooze();
    void stopAlarm();
    void startAlarm(AlarmObject* alarmObject);

    Q_INVOKABLE void setAlarmEnabled(const QString& alarmUuid, bool enabled);
    Q_INVOKABLE void setGroupEnabled(const QString& groupUuid, bool enabled);
    Q_INVOKABLE void removeGroup(const QString& groupUuid);
    Q_INVOKABLE void assignAlarmsToGroup(const QStringList& alarmUuids, const QString& groupUuid);

signals:
    void startRinging(AlarmObject* alarmObject);
    void stopRinging();

private:
    AlarmObject* alarm(const QString& uuid) const;
    AlarmGroupObject* group(const QString& uuid) const;
    int alarmDetailInt(const QString& key, int fallback) const;
    bool alarmDetailBool(const QString& key, bool fallback) const;
    void updateGroupStates();

    QTimer* m_ringTimer=nullptr;
    QTimer* m_snoozeTimer=nullptr;
    QPointer<AlarmObject> m_activeAlarm;
};

#endif // CLOCK_H
