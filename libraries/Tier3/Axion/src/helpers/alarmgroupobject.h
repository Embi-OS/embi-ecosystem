#ifndef ALARMGROUPOBJECT_H
#define ALARMGROUPOBJECT_H

#include <QDefs>
#include <QModels>

class AlarmGroupModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit AlarmGroupModel(QObject* parent = nullptr);

    Q_INVOKABLE void create(const QVariantMap& alarmGroupMap);
};

class AlarmGroupObject: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, uuid, Uuid, "")

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, details, Details, {})
    Q_READONLY_VAR_PROPERTY(bool, enabled, Enabled, false)
    Q_READONLY_VAR_PROPERTY(int, enabledAlarmCount, EnabledAlarmCount, 0)
    Q_READONLY_VAR_PROPERTY(int, disabledAlarmCount, DisabledAlarmCount, 0)

public:
    explicit AlarmGroupObject(QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap toMap() const;
    Q_INVOKABLE void fromMap(const QVariantMap& alarmGroupMap);
};

#endif // ALARMGROUPOBJECT_H
