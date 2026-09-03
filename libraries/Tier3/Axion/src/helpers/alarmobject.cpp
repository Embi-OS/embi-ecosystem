#include "alarmobject.h"
#include "axion_log.h"

#include "qmodelmatcher.h"

#include <algorithm>

AlarmModel::AlarmModel(QObject* parent) :
    QObjectListModel(parent, &AlarmObject::staticMetaObject)
{
    onInserted<AlarmObject>([this](AlarmObject* alarmObject){
        connect(alarmObject, &AlarmObject::ringing, this, [this, alarmObject](){ emit this->ringing(alarmObject); });
        connect(alarmObject, &AlarmObject::remainingTimeChanged, this, &AlarmModel::invalidateRemainingTimeChange);
        invalidateRemainingTimeChange();
    });
    onRemoved<AlarmObject>([this](AlarmObject*){
        invalidateRemainingTimeChange();
    });

    QModelMatcher* loadingMatcher = new QModelMatcher(this);
    loadingMatcher->setSourceModel(this);
    loadingMatcher->setRoleName("enabled");
    loadingMatcher->setValue(true);
    loadingMatcher->setHits(1);
    loadingMatcher->setDelayed(true);
    connect(loadingMatcher, &QModelMatcher::countChanged, this, &AlarmModel::setEnabled);
    loadingMatcher->invalidate();
}

void AlarmModel::invalidateRemainingTimeChange()
{
    qint64 msToNextRingTime=-1;
    QDateTime nextRingDateTime;
    for(AlarmObject* alarmObject: this->modelIterator<AlarmObject>())
    {
        if(alarmObject->getEnabled() && (msToNextRingTime<0 || alarmObject->getMsToNextRingTime()<msToNextRingTime))
        {
            msToNextRingTime = alarmObject->getMsToNextRingTime();
            nextRingDateTime = alarmObject->getNextRingDateTime();
        }
    }

    setNextRingDateTime(nextRingDateTime);
    setMsToNextRingTime(msToNextRingTime);
    if(m_msToNextRingTime<0)
        setNextTimeRing(tr("Toutes les alarmes sont désactivées"));
    else
        setNextTimeRing(tr("Alarme dans %1 - %2").arg(AlarmObject::nextTimeRing(m_msToNextRingTime/1000), QLocale().toString(m_nextRingDateTime.toLocalTime(), QLocale::NarrowFormat)));
}

void AlarmModel::create(const QVariantMap& alarmMap)
{
    AlarmObject* alarmObject = new AlarmObject(this);
    alarmObject->fromMap(alarmMap);
    append(alarmObject);
}

AlarmObject::AlarmObject(QObject *parent):
    QObject(parent),
    m_timer(new QTimer(this)),
    m_remainingTimeChangeCaller(new QTimer(this))
{
    connect(this, &AlarmObject::hourChanged, this, &AlarmObject::queueInvalidate);
    connect(this, &AlarmObject::minuteChanged, this, &AlarmObject::queueInvalidate);
    connect(this, &AlarmObject::dateChanged, this, &AlarmObject::queueInvalidate);
    connect(this, &AlarmObject::enabledChanged, this, &AlarmObject::queueInvalidate);
    connect(this, &AlarmObject::repeatChanged, this, &AlarmObject::queueInvalidate);
    connect(this, &AlarmObject::weekdaysChanged, this, &AlarmObject::queueInvalidate);

    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &AlarmObject::onTimerTimeout);

    m_remainingTimeChangeCaller->setSingleShot(false);
    m_remainingTimeChangeCaller->setInterval(1000);
    connect(m_remainingTimeChangeCaller, &QTimer::timeout, this, &AlarmObject::invalidateRemainingTimeChange);
    invalidateRemainingTimeChange();
}

QVariantMap AlarmObject::toMap() const
{
    QVariantMap map;

    map.insert("uuid", m_uuid);
    map.insert("group", m_group);
    map.insert("hour", m_hour);
    map.insert("minute", m_minute);
    map.insert("date", m_date);
    map.insert("enabled", m_enabled);
    map.insert("name", m_name);
    map.insert("repeat", m_repeat);
    map.insert("weekdays", m_weekdays);
    map.insert("details", m_details);

    return map;
}

void AlarmObject::fromMap(const QVariantMap& alarmMap)
{
    if(alarmMap.contains("uuid"))
        setUuid(alarmMap.value("uuid").toString());
    if(alarmMap.contains("group"))
        setGroup(alarmMap.value("group").toString());
    if(alarmMap.contains("hour"))
    {
        const int hour = alarmMap.value("hour").toInt();
        if(hour >= 0 && hour < 24)
            setHour(hour);
    }
    if(alarmMap.contains("minute"))
    {
        const int minute = alarmMap.value("minute").toInt();
        if(minute >= 0 && minute < 60)
            setMinute(minute);
    }
    if(alarmMap.contains("date"))
        setDate(alarmMap.value("date").toDate());
    if(alarmMap.contains("name"))
        setName(alarmMap.value("name").toString());
    if(alarmMap.contains("repeat"))
        setRepeat(alarmMap.value("repeat").toBool());
    if(alarmMap.contains("weekdays"))
        setWeekdays(alarmMap.value("weekdays").toInt() & WeekdayMask);
    if(alarmMap.contains("details"))
        setDetails(alarmMap.value("details").toMap());

    if(alarmMap.contains("enabled"))
        setEnabled(alarmMap.value("enabled").toBool());
}

void AlarmObject::ring()
{
    m_remainingTimeChangeCaller->stop();
    m_timer->stop();

    if(!m_enabled)
        return;

    AXIONLOG_INFO()<<"Ringing alarm"<<m_name;

    emit this->ringing();

    setEnabled(m_repeat);
    queueInvalidate();
}

void AlarmObject::cancel()
{
    m_remainingTimeChangeCaller->stop();
    m_timer->stop();

    if(!m_enabled)
        return;
    setEnabled(false);

    AXIONLOG_INFO()<<"Cancel alarm"<<m_name;
}

void AlarmObject::queueInvalidate()
{
    if(m_invalidateQueued)
        return;
    m_invalidateQueued = true;

    m_remainingTimeChangeCaller->stop();
    m_timer->stop();

    QTimer::singleShot(1000, this, [this](){
        invalidate();
    });
}

void AlarmObject::invalidate()
{
    m_remainingTimeChangeCaller->stop();
    m_timer->stop();

    invalidateRemainingTimeChange();

    const qint64 delay = m_msToNextRingTime;
    if(m_enabled && delay>0)
        startTimer(delay);
    else if(m_enabled)
    {
        setEnabled(false);
        invalidateRemainingTimeChange();
    }

    emit this->invalidated();
    m_invalidateQueued = false;

    if(m_timer->isActive() && m_timer->remainingTime()>1000)
        m_remainingTimeChangeCaller->start();
}

void AlarmObject::startTimer(qint64 delay)
{
    if(delay <= 0)
        return;

    m_timer->start(static_cast<int>((std::min)(delay, MaximumTimerInterval)));
}

void AlarmObject::onTimerTimeout()
{
    m_timer->stop();
    invalidateRemainingTimeChange();

    if(m_enabled && m_msToNextRingTime > 0)
    {
        startTimer(m_msToNextRingTime);
        if(m_timer->remainingTime() > 1000)
            m_remainingTimeChangeCaller->start();
        return;
    }

    ring();
}

QString AlarmObject::nextTimeRing(qint64 secToNextRingTime, QLocale::FormatType format)
{
    if(secToNextRingTime<=0)
        return "N/A";

    qint64 day = secToNextRingTime / (24 * 3600);
    qint64 hour = secToNextRingTime / 3600 - day * 24;
    qint64 minute = secToNextRingTime / 60 - day * 24 * 60 - hour * 60;
    QString arg;
    if (day > 0) {
        if(format==QLocale::NarrowFormat && (hour > 0 || minute > 0))
            day += 1;
        arg += tr("%1 jour(s)").arg(day);
        if(format==QLocale::NarrowFormat)
            return arg;
    }
    if (hour > 0) {
        if (day > 0 && minute > 0) {
            arg += ", ";
        } else if (day > 0) {
            arg += tr(" et ");
        }
        arg += tr("%1 heure(s)").arg(hour);
    }
    if (minute > 0) {
        if (day > 0 || hour > 0) {
            arg += tr(" et ");
        }
        arg += tr("%1 minute(s)").arg(minute);
    }

    if (day <= 0 && hour <= 0 && minute <= 0) {
        return tr("Moins d'une minute");
    } else {
        return arg;
    }
}

void AlarmObject::invalidateRemainingTimeChange()
{
    if(m_timer->isActive() && m_timer->remainingTime()<1000)
        return;

    const QDateTime now = QDateTime::currentDateTime();

    setNextRingDateTime(calculateNextRingDateTime());
    setMsToNextRingTime(now.msecsTo(m_nextRingDateTime));
    setNextTimeRing(nextTimeRing(m_msToNextRingTime/1000));

    if(m_timer->isActive() && m_msToNextRingTime <= MaximumTimerInterval
            && qAbs(qint64(m_timer->remainingTime())-m_msToNextRingTime)>1000)
    {
        AXIONLOG_WARNING()<<"AlarmObject.remainingTime differs from msToNextRingTime:"<<qAbs(qint64(m_timer->remainingTime())-m_msToNextRingTime)/1000.0<<"sec";
        startTimer(m_msToNextRingTime);
    }

    emit this->remainingTimeChanged();
}

QDateTime AlarmObject::calculateNextRingDateTime() const
{
    // if not enabled, means this would never ring
    if (!m_enabled)
        return QDateTime();

    const QDateTime now = QDateTime::currentDateTime();
    const QTime alarmTime(m_hour, m_minute, 0);
    if(!alarmTime.isValid())
        return QDateTime();

    if(m_repeat)
    {
        const int weekdays = m_weekdays & WeekdayMask;
        if(!weekdays)
            return QDateTime();

        for(int dayOffset=0; dayOffset<7; ++dayOffset)
        {
            const QDate date = now.date().addDays(dayOffset);
            if((weekdays & (1 << (date.dayOfWeek() - 1))) == 0)
                continue;

            const QDateTime alarmDateTime(date, alarmTime);
            if(alarmDateTime > now)
                return alarmDateTime;
        }

        return QDateTime();
    }

    if(!m_date.isValid())
        return QDateTime();

    const QDateTime alarmDateTime(m_date, alarmTime);
    if(alarmDateTime <= now || !alarmDateTime.isValid())
        return QDateTime();

    return alarmDateTime;
}
