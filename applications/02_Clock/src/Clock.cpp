#include "Clock.h"
#include "ClockAlarmAudio.h"
#include "ClockDisplay.h"

#include <Rest>

#include "utils/datetimeutils.h"

Clock::Clock(QObject *parent) :
    AbstractManager(parent),
    m_alarmModel(new AlarmModel(this)),
    m_alarmGroupModel(new AlarmGroupModel(this)),
    m_ringTimer(new QTimer(this)),
    m_snoozeTimer(new QTimer(this))
{
    m_alarmModel->onInserted<AlarmObject>(this, [](AlarmObject* alarmObject) {
        RestMapper* mapper = new RestMapper(alarmObject);
        mapper->setSyncable(true);
        mapper->setSelectPolicy(QVariantMapperPolicies::Direct);
        mapper->setSubmitPolicy(QVariantMapperPolicies::Delayed);
        mapper->setBaseName("api/alarm");

        if(alarmObject->getUuid().isEmpty()) {
            alarmObject->setUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
            mapper->setBody(alarmObject->toMap());
        }
        mapper->setMethod(alarmObject->getUuid());

        connect(alarmObject, &AlarmObject::uuidChanged, mapper, &RestMapper::setMethod);
        connect(mapper, &RestMapper::selectDone, mapper, &RestMapper::resetBody, Qt::SingleShotConnection);

        mapper->mapProperty(alarmObject, "group", "group");
        mapper->mapProperty(alarmObject, "name", "name");
        mapper->mapProperty(alarmObject, "enabled", "enabled");
        mapper->mapProperty(alarmObject, "hour", "hour");
        mapper->mapProperty(alarmObject, "minute", "minute");
        mapper->mapProperty(alarmObject, "date", "date");
        mapper->mapProperty(alarmObject, "repeat", "repeat");
        mapper->mapProperty(alarmObject, "weekdays", "weekdays");
        mapper->mapProperty(alarmObject, "details", "details");

        RestSocketAttached::wrap(mapper)->setEnabled(true);

        mapper->select();
    });
    m_alarmModel->onRemoved<AlarmObject>(this, [](AlarmObject* alarmObject) {
        RestHelper::defaultApiRootClass()->deleteResource(QString("api/alarm/%1").arg(alarmObject->getUuid()));
    });
    m_alarmModel->onInserted<AlarmObject>(this, [this](AlarmObject* alarmObject) {
        connect(alarmObject, &AlarmObject::enabledChanged, this, &Clock::updateGroupStates);
        connect(alarmObject, &AlarmObject::groupChanged, this, &Clock::updateGroupStates);
        updateGroupStates();
    });
    m_alarmModel->onRemoved<AlarmObject>(this, [this](AlarmObject*) {
        updateGroupStates();
    });

    m_alarmGroupModel->onInserted<AlarmGroupObject>(this, [](AlarmGroupObject* alarmGroupObject) {
        RestMapper* mapper = new RestMapper(alarmGroupObject);
        mapper->setSyncable(true);
        mapper->setSelectPolicy(QVariantMapperPolicies::Direct);
        mapper->setSubmitPolicy(QVariantMapperPolicies::Delayed);
        mapper->setBaseName("api/group");

        if(alarmGroupObject->getUuid().isEmpty()) {
            alarmGroupObject->setUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
            mapper->setBody(alarmGroupObject->toMap());
        }
        mapper->setMethod(alarmGroupObject->getUuid());

        connect(alarmGroupObject, &AlarmGroupObject::uuidChanged, mapper, &RestMapper::setMethod);
        connect(mapper, &RestMapper::selectDone, mapper, &RestMapper::resetBody, Qt::SingleShotConnection);

        mapper->mapProperty(alarmGroupObject, "name", "name");
        mapper->mapProperty(alarmGroupObject, "details", "details");

        RestSocketAttached::wrap(mapper)->setEnabled(true);

        mapper->select();
    });
    m_alarmGroupModel->onRemoved<AlarmGroupObject>(this, [](AlarmGroupObject* alarmGroupObject) {
        RestHelper::defaultApiRootClass()->deleteResource(QString("api/group/%1").arg(alarmGroupObject->getUuid()));
    });
    m_alarmGroupModel->onInserted<AlarmGroupObject>(this, [this](AlarmGroupObject*) {
        updateGroupStates();
    });

    connect(m_alarmModel, &AlarmModel::ringing, this, [this](AlarmObject* alarmObject) {
        qTrace() << "[Clock] Alarm ringing signal received" << (alarmObject ? alarmObject->getUuid() : QString());
        startAlarm(alarmObject);
    });

    m_ringTimer->setSingleShot(true);
    connect(m_ringTimer, &QTimer::timeout, this, [this]() {
        qTrace() << "[Clock] Alarm auto-dismiss timer elapsed";
        stopAlarm();
    });

    m_snoozeTimer->setSingleShot(true);

    connect(m_snoozeTimer, &QTimer::timeout, this, [this](){
        qTrace() << "[Clock] Snooze timer elapsed" << (m_activeAlarm ? m_activeAlarm->getUuid() : QString());
        startAlarm(m_activeAlarm);
    });
}

bool Clock::init()
{
    connect(this, &Clock::startRinging, ClockAlarmAudio::Get(), &ClockAlarmAudio::startAlarm, Qt::QueuedConnection);
    connect(this, &Clock::stopRinging, ClockAlarmAudio::Get(), &ClockAlarmAudio::stopAlarm, Qt::QueuedConnection);

    ClockDisplay::Get()->setWakeTimeout(m_wakeTimeout);
    connect(this, &Clock::wakeTimeoutChanged, ClockDisplay::Get(), &ClockDisplay::setWakeTimeout);

    ClockAlarmAudio::Get()->setDefaultVolume(m_defaultVolume);
    connect(this, &Clock::defaultVolumeChanged, ClockAlarmAudio::Get(), &ClockAlarmAudio::setDefaultVolume);

    return true;
}

bool Clock::postInit()
{
    RestReply* replyGroup = RestHelper::defaultApiRootClass()->get(QString("api/group"));
    replyGroup->onSucceeded(this, [this](int, const QVariant& reply) {
        const QVariantList alarmGroups = qVariantGetNestedValue(reply, "data").toList();
        for(const QVariant& alarmGroup: alarmGroups)
            m_alarmGroupModel->create(alarmGroup.toMap());
    });
    replyGroup->onAllErrors([](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant&) {
        QVariantMap settings;
        settings.insert("caption", tr("Erreur lors de l'initialisation des groupes d'alarmes"));
        settings.insert("details", errorString);
        settings.insert("diagnose", false);
        SnackbarManager::Get()->showCritical(settings);
    });

    RestReply* replyAlarm = RestHelper::defaultApiRootClass()->get(QString("api/alarm"));
    replyAlarm->onSucceeded(this, [this](int, const QVariant& reply) {
        const QVariantList alarms = qVariantGetNestedValue(reply, "data").toList();
        for(const QVariant& alarm: alarms)
            m_alarmModel->create(alarm.toMap());
    });
    replyAlarm->onAllErrors([](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant&) {
        QVariantMap settings;
        settings.insert("caption", tr("Erreur lors de l'initialisation des alarmes"));
        settings.insert("details", errorString);
        settings.insert("diagnose", false);
        SnackbarManager::Get()->showCritical(settings);
    });

    return true;
}

void Clock::snooze()
{
    if(!m_activeAlarm || !m_snoozeAvailable)
    {
        qWarning() << "[Clock] Snooze ignored: no active alarm or snooze disabled";
        return;
    }

    m_ringTimer->stop();
    m_snoozeTimer->stop();

    emit this->stopRinging();

    const int snoozeDuration = alarmDetailInt("behavior.snoozeDuration", m_snoozeTimeout);
    qTrace() << "[Clock] Snoozing alarm" << m_activeAlarm->getUuid() << "for" << snoozeDuration << "minutes";
    m_snoozeTimer->start(snoozeDuration * 60 * 1000);

    setRinging(false);
    setSnoozed(true);
}

void Clock::stopAlarm()
{
    qTrace() << "[Clock] Stopping alarm" << (m_activeAlarm ? m_activeAlarm->getUuid() : QString());
    m_ringTimer->stop();
    m_snoozeTimer->stop();

    emit this->stopRinging();

    setRinging(false);
    setSnoozed(false);
    setSnoozeAvailable(false);
    m_activeAlarm = nullptr;

    ClockDisplay::Get()->setWakeTimeout(m_wakeTimeout);
}

void Clock::startAlarm(AlarmObject* alarmObject)
{
    if(!alarmObject)
    {
        qWarning() << "[Clock] Alarm start ignored: null alarm object";
        return;
    }

    qTrace() << "[Clock] Starting alarm" << alarmObject->getUuid() << "details" << alarmObject->getDetails();

    m_activeAlarm = alarmObject;
    setRinging(true);
    setSnoozed(false);
    setSnoozeAvailable(alarmDetailBool("behavior.snoozeEnabled", false));

    ClockDisplay::Get()->setWakeTimeout(alarmDetailInt("display.wakeTimeout", m_wakeTimeout));
    ClockDisplay::Get()->gotoWaking();

    qTrace() << "[Clock] Dispatching audio start for alarm" << alarmObject->getUuid();
    emit this->startRinging(alarmObject);

    m_ringTimer->stop();
    m_snoozeTimer->stop();

    const int ringTimeout = alarmDetailInt("behavior.autoDismissDuration", m_ringTimeout);
    if(ringTimeout > 0)
    {
        qTrace() << "[Clock] Auto-dismiss scheduled in" << ringTimeout << "minutes";
        m_ringTimer->start(ringTimeout * 60 * 1000);
    }
    else
        qTrace() << "[Clock] Alarm requires manual dismissal";
}

void Clock::setAlarmEnabled(const QString& alarmUuid, bool enabled)
{
    AlarmObject* alarmObject = alarm(alarmUuid);
    if(!alarmObject || alarmObject->getEnabled() == enabled)
        return;

    QVariantMap alarmMap{{"enabled", enabled}};
    if(!alarmObject->getRepeat())
        alarmMap.insert("date", DateTimeUtils::nextValidDateTimeForTime(alarmObject->getHour(), alarmObject->getMinute(), 0, 0));

    alarmObject->fromMap(alarmMap);
}

void Clock::setGroupEnabled(const QString& groupUuid, bool enabled)
{
    if(groupUuid.isEmpty() || !group(groupUuid))
        return;

    for(QObject* object : m_alarmModel->objects())
    {
        AlarmObject* alarmObject = qobject_cast<AlarmObject*>(object);
        if(!alarmObject || alarmObject->getGroup() != groupUuid || alarmObject->getEnabled() == enabled)
            continue;

        QVariantMap alarmMap{{"enabled", enabled}};
        if(enabled && !alarmObject->getRepeat())
            alarmMap.insert("date", DateTimeUtils::nextValidDateTimeForTime(alarmObject->getHour(), alarmObject->getMinute(), 0, 0));

        alarmObject->fromMap(alarmMap);
    }
}

void Clock::removeGroup(const QString& groupUuid)
{
    AlarmGroupObject* groupObject = group(groupUuid);
    if(!groupObject)
        return;

    for(QObject* object : m_alarmModel->objects())
    {
        AlarmObject* alarmObject = qobject_cast<AlarmObject*>(object);
        if(alarmObject && alarmObject->getGroup() == groupUuid)
            alarmObject->fromMap({{"group", ""}});
    }

    m_alarmGroupModel->remove(groupObject);
}

void Clock::assignAlarmsToGroup(const QStringList& alarmUuids, const QString& groupUuid)
{
    if(!groupUuid.isEmpty() && !group(groupUuid))
        return;

    for(const QString& alarmUuid : alarmUuids)
    {
        AlarmObject* alarmObject = alarm(alarmUuid);
        if(alarmObject && alarmObject->getGroup() != groupUuid)
            alarmObject->fromMap({{"group", groupUuid}});
    }
}

AlarmObject* Clock::alarm(const QString& uuid) const
{
    for(QObject* object : m_alarmModel->objects())
    {
        AlarmObject* alarmObject = qobject_cast<AlarmObject*>(object);
        if(alarmObject && alarmObject->getUuid() == uuid)
            return alarmObject;
    }

    return nullptr;
}

AlarmGroupObject* Clock::group(const QString& uuid) const
{
    for(QObject* object : m_alarmGroupModel->objects())
    {
        AlarmGroupObject* groupObject = qobject_cast<AlarmGroupObject*>(object);
        if(groupObject && groupObject->getUuid() == uuid)
            return groupObject;
    }

    return nullptr;
}

int Clock::alarmDetailInt(const QString& key, int fallback) const
{
    if(!m_activeAlarm)
        return fallback;

    const QVariant value = qVariantGetNestedValue(m_activeAlarm->getDetails(), key);
    return value.isValid() ? qMax(0, value.toInt()) : fallback;
}

bool Clock::alarmDetailBool(const QString& key, bool fallback) const
{
    if(!m_activeAlarm)
        return fallback;

    const QVariant value = qVariantGetNestedValue(m_activeAlarm->getDetails(), key);
    return value.isValid() ? value.toBool() : fallback;
}

void Clock::updateGroupStates()
{
    QHash<QString, QPair<int, int>> counts;
    for(QObject* object : m_alarmModel->objects())
    {
        AlarmObject* alarmObject = qobject_cast<AlarmObject*>(object);
        if(!alarmObject || alarmObject->getGroup().isEmpty())
            continue;

        QPair<int, int>& groupCounts = counts[alarmObject->getGroup()];
        if(alarmObject->getEnabled())
            ++groupCounts.first;
        else
            ++groupCounts.second;
    }

    for(QObject* object : m_alarmGroupModel->objects())
    {
        AlarmGroupObject* groupObject = qobject_cast<AlarmGroupObject*>(object);
        if(!groupObject)
            continue;

        const QPair<int, int> groupCounts = counts.value(groupObject->getUuid());
        groupObject->setEnabledAlarmCount(groupCounts.first);
        groupObject->setDisabledAlarmCount(groupCounts.second);
        groupObject->setEnabled(groupCounts.first > 0);
    }
}
