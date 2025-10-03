#include "Clock.h"
#include "ClockDisplay.h"
#include "ClockMedia.h"

#include <Rest>

Clock::Clock(QObject *parent) :
    AbstractManager(parent),
    m_alarmModel(new AlarmModel(this)),
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

        mapper->mapProperty(alarmObject, "name", "name");
        mapper->mapProperty(alarmObject, "enabled", "enabled");
        mapper->mapProperty(alarmObject, "hour", "hour");
        mapper->mapProperty(alarmObject, "minute", "minute");
        mapper->mapProperty(alarmObject, "date", "date");
        mapper->mapProperty(alarmObject, "repeat", "repeat");
        mapper->mapProperty(alarmObject, "weekdays", "weekdays");

        RestSocketAttached::wrap(mapper)->setEnabled(true);

        mapper->select();
    });
    m_alarmModel->onRemoved<AlarmObject>(this, [](AlarmObject* alarmObject) {
        RestHelper::defaultApiRootClass()->deleteResource(QString("api/alarm/%1").arg(alarmObject->getUuid()));
    });

    connect(m_alarmModel, &AlarmModel::ringing, this, &Clock::startAlarm);

    m_ringTimer->setSingleShot(true);
    m_ringTimer->setInterval(m_ringTimeout * 60 * 1000);

    connect(m_ringTimer, &QTimer::timeout, this, &Clock::stopAlarm);
    connect(this, &Clock::ringTimeoutChanged, m_ringTimer, [this](int timeout){ m_ringTimer->setInterval(timeout * 60 * 1000); });

    m_snoozeTimer->setSingleShot(true);
    m_snoozeTimer->setInterval(m_snoozeTimeout * 60 * 1000);

    connect(m_snoozeTimer, &QTimer::timeout, this, &Clock::startAlarm);
    connect(this, &Clock::snoozeTimeoutChanged, m_snoozeTimer, [this](int timeout){ m_snoozeTimer->setInterval(timeout * 60 * 1000); });
}

bool Clock::init()
{
    connect(this, &Clock::startRinging, ClockDisplay::Get(), &ClockDisplay::gotoWaking);
    connect(this, &Clock::startRinging, ClockMedia::Get(), &ClockMedia::startMedia, Qt::QueuedConnection);
    connect(this, &Clock::stopRinging, ClockMedia::Get(), &ClockMedia::stopMedia, Qt::QueuedConnection);

    ClockDisplay::Get()->setWakeTimeout(m_wakeTimeout);
    connect(this, &Clock::wakeTimeoutChanged, ClockDisplay::Get(), &ClockDisplay::setWakeTimeout);

    ClockMedia::Get()->setWakeTimeout(m_wakeTimeout);
    connect(this, &Clock::wakeTimeoutChanged, ClockMedia::Get(), &ClockMedia::setWakeTimeout);

    ClockMedia::Get()->setDefaultVolume(m_defaultVolume);
    connect(this, &Clock::defaultVolumeChanged, ClockMedia::Get(), &ClockMedia::setDefaultVolume);

    return true;
}

void Clock::postInit()
{
    RestModel* alarmModel = new RestModel(this);
    alarmModel->setPrimaryField("uuid");
    alarmModel->setBaseName("api/alarm");
    alarmModel->setSelectPolicy(QVariantListModelPolicies::Direct);
    alarmModel->setSubmitPolicy(QVariantListModelPolicies::Disabled);

    connect(alarmModel, &RestModel::selectDone, this, [this, alarmModel](bool result){

        if(!result)
        {
            emitInitDone(result);
            return;
        }

        for(int i=0; i<alarmModel->size(); i++)
            m_alarmModel->create(alarmModel->get(i).toMap());

        emitInitDone(result);

    }, Qt::SingleShotConnection);

    alarmModel->select();
}

void Clock::snooze()
{
    m_ringTimer->stop();
    m_snoozeTimer->stop();

    m_snoozeTimer->start();

    setRinging(false);
    setSnoozed(true);
}

void Clock::stopAlarm()
{
    m_ringTimer->stop();

    emit this->stopRinging();

    setRinging(false);
    setSnoozed(false);
}

void Clock::startAlarm()
{
    setRinging(true);
    setSnoozed(false);

    emit this->startRinging();

    m_ringTimer->stop();
    m_snoozeTimer->stop();

    m_ringTimer->start();
}
