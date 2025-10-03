#ifndef ALARMOBJECT_H
#define ALARMOBJECT_H

#include <QDefs>
#include <QModels>
#include <QProxyModel>

class AlarmModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, false)

    Q_READONLY_VAR_PROPERTY(int, msToNextRingTime, MsToNextRingTime, 0)
    Q_READONLY_REF_PROPERTY(QDateTime, nextRingDateTime, NextRingDateTime, {})
    Q_READONLY_REF_PROPERTY(QString, nextTimeRing, NextTimeRing, {})

public:
    explicit AlarmModel(QObject* parent = nullptr);

    Q_INVOKABLE void create(const QVariantMap& alarmMap);

signals:
    void remainingTimeChanged();
    void ringing();

private slots:
    void invalidateRemainingTimeChange();
};

class AlarmObject: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, uuid, Uuid, "")

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, false)
    Q_WRITABLE_VAR_PROPERTY(int, hour, Hour, 0)
    Q_WRITABLE_VAR_PROPERTY(int, minute, Minute, 0)
    Q_WRITABLE_REF_PROPERTY(QDate, date, Date, {})
    Q_WRITABLE_VAR_PROPERTY(bool, repeat, Repeat, false)
    Q_WRITABLE_VAR_PROPERTY(int, weekdays, Weekdays, 0)

    Q_READONLY_VAR_PROPERTY(int, msToNextRingTime, MsToNextRingTime, 0)
    Q_READONLY_REF_PROPERTY(QDateTime, nextRingDateTime, NextRingDateTime, {})
    Q_READONLY_REF_PROPERTY(QString, nextTimeRing, NextTimeRing, {})

public:
    explicit AlarmObject(QObject *parent = nullptr);

    static QString nextTimeRing(int secToNextRingTime, QLocale::FormatType format=QLocale::LongFormat);

    Q_INVOKABLE virtual QVariantMap toMap() const;
    Q_INVOKABLE virtual void fromMap(const QVariantMap& alarmMap);

public slots:
    void ring();
    void cancel();

    void queueInvalidate();
    void invalidate();

signals:
    void remainingTimeChanged();
    void invalidated();

    void ringing();

private slots:
    void invalidateRemainingTimeChange();

private:
    QDateTime calculateNextRingDateTime() const;

    bool m_invalidateQueued = false;

    QTimer* m_timer = nullptr;
    QTimer* m_remainingTimeChangeCaller = nullptr;
};

#endif // ALARMOBJECT_H
