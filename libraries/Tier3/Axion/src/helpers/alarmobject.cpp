#include "alarmobject.h"
#include "axion_log.h"

AlarmModel::AlarmModel(QObject* parent) :
    QObjectListModel(parent, &AlarmObject::staticMetaObject)
{
    onInserted<AlarmObject>([this](AlarmObject* alarmObject){
        connect(alarmObject, &AlarmObject::ringing, this, &AlarmModel::ringing);
        connect(alarmObject, &AlarmObject::remainingTimeChanged, this, &AlarmModel::invalidateRemainingTimeChange);
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
    int msToNextRingTime=-1;
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
        setNextTimeRing(tr("Alarme dans %1 - %2").arg(AlarmObject::nextTimeRing(m_msToNextRingTime/1000), QLocale().toString(m_nextRingDateTime, QLocale::NarrowFormat)));
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
    connect(m_timer, &QTimer::timeout, this, &AlarmObject::ring);

    m_remainingTimeChangeCaller->setSingleShot(false);
    m_remainingTimeChangeCaller->setInterval(1000);
    connect(m_remainingTimeChangeCaller, &QTimer::timeout, this, &AlarmObject::invalidateRemainingTimeChange);
    invalidateRemainingTimeChange();
}

QVariantMap AlarmObject::toMap() const
{
    QVariantMap map;

    map.insert("uuid", m_uuid);
    map.insert("hour", m_hour);
    map.insert("minute", m_minute);
    map.insert("date", m_date);
    map.insert("enabled", m_enabled);
    map.insert("name", m_name);
    map.insert("repeat", m_repeat);
    map.insert("weekdays", m_weekdays);

    return map;
}

void AlarmObject::fromMap(const QVariantMap& alarmMap)
{
    if(alarmMap.contains("uuid"))
        setUuid(alarmMap.value("uuid").toString());
    if(alarmMap.contains("hour"))
        setHour(alarmMap.value("hour").toInt());
    if(alarmMap.contains("minute"))
        setMinute(alarmMap.value("minute").toInt());
    if(alarmMap.contains("date"))
        setDate(alarmMap.value("date").toDate());
    if(alarmMap.contains("enabled"))
        setEnabled(alarmMap.value("enabled").toBool());
    if(alarmMap.contains("name"))
        setName(alarmMap.value("name").toString());
    if(alarmMap.contains("repeat"))
        setRepeat(alarmMap.value("repeat").toBool());
    if(alarmMap.contains("weekdays"))
        setWeekdays(alarmMap.value("weekdays").toInt());
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

    int delay = m_msToNextRingTime;
    if(m_enabled && delay>0)
        m_timer->start(delay);
    else
        setEnabled(false);

    emit this->invalidated();
    m_invalidateQueued = false;

    if(m_timer->isActive() && m_timer->remainingTime()>1000)
        m_remainingTimeChangeCaller->start();
}

QString AlarmObject::nextTimeRing(int secToNextRingTime, QLocale::FormatType format)
{
    if(secToNextRingTime<=0)
        return "N/A";

    int day = secToNextRingTime / (24 * 3600);
    int hour = secToNextRingTime / 3600 - day * 24;
    int minute = secToNextRingTime / 60 - day * 24 * 60 - hour * 60;
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

    if(m_timer->isActive() && (m_timer->remainingTime()-m_msToNextRingTime)>1000)
    {
        AXIONLOG_WARNING()<<"AlarmObject.remainingTime differs from msToNextRingTime:"<<qAbs(m_timer->remainingTime()-m_msToNextRingTime)/1000.0<<"sec";
        m_timer->start(m_msToNextRingTime);
    }

    emit this->remainingTimeChanged();
}

QDateTime AlarmObject::calculateNextRingDateTime() const
{
    // if not enabled, means this would never ring
    if (!m_enabled)
        return QDateTime();

    // alarm is not valid
    if (m_repeat && !m_weekdays)
        return QDateTime();

    const QDateTime now = QDateTime::currentDateTime();
    QDateTime alarmDateTime;

    // alarm does not repeat (no days of the week are specified)
    if (m_weekdays == 0 && m_date.isValid())
    {
        // get the time when the alarm will ring
        alarmDateTime = QDateTime(m_date,QTime(m_hour, m_minute, 0));
    }
    else if (m_weekdays > 0) // repeat alarm
    {
        bool first = true;

        // get the time when the alarm will ring
        const QTime alarmTime = QTime(m_hour, m_minute, 0);
        QDateTime dateToRing = QDateTime::currentDateTime();
        // keeping looping forward a single day until the day of week is accepted
        while (((m_weekdays & (1<<(dateToRing.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < dateToRing.time()))) // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            dateToRing = dateToRing.addDays(1); // go forward a day
            first = false;
        }
        alarmDateTime = QDateTime(dateToRing.date(), alarmTime);

        return alarmDateTime;
    }

    // alarm is in the past
    if(alarmDateTime <= now || !alarmDateTime.isValid())
        return QDateTime();

    return alarmDateTime;
}
