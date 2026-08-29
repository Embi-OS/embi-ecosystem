#include "alarmgroupobject.h"
#include "axion_log.h"

AlarmGroupModel::AlarmGroupModel(QObject* parent) :
    QObjectListModel(parent, &AlarmGroupObject::staticMetaObject)
{

}

void AlarmGroupModel::create(const QVariantMap& alarmGroupMap)
{
    AlarmGroupObject* alarmGroupObject = new AlarmGroupObject(this);
    alarmGroupObject->fromMap(alarmGroupMap);
    append(alarmGroupObject);
}

AlarmGroupObject::AlarmGroupObject(QObject *parent):
    QObject(parent)
{

}

QVariantMap AlarmGroupObject::toMap() const
{
    QVariantMap map;

    map.insert("uuid", m_uuid);
    map.insert("name", m_name);
    map.insert("details", m_details);

    return map;
}

void AlarmGroupObject::fromMap(const QVariantMap& alarmGroupMap)
{
    if(alarmGroupMap.contains("uuid"))
        setUuid(alarmGroupMap.value("uuid").toString());
    if(alarmGroupMap.contains("name"))
        setName(alarmGroupMap.value("name").toString());
    if(alarmGroupMap.contains("details"))
        setDetails(alarmGroupMap.value("details").toMap());
}
