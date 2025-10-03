#include "datetimeutils.h"
#include <QFontDatabase>
#include <QUtils>

DateTimeUtils::DateTimeUtils(QObject *parent) :
    QObject(parent),
    m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &DateTimeUtils::refreshDateTime);

    refreshDateTime();
}

QDateTime DateTimeUtils::getSystemDateTime() const
{
    return QDateTime::currentDateTime();
}

QDate DateTimeUtils::getSystemDate() const
{
    return getSystemDateTime().date();
}

QTime DateTimeUtils::getSystemTime() const
{
    return getSystemDateTime().time();
}

void DateTimeUtils::refreshDateTime()
{
    m_timer->stop();

    QDateTime datetime = QDateTime::currentDateTime();

    setYear(datetime.date().year());
    setMonth(datetime.date().month());
    setDay(datetime.date().day());
    setHour(datetime.time().hour());
    setMinute(datetime.time().minute());
    setSecond(datetime.time().second());

    emit this->systemDateTimeChanged();

    // TimeDate will be refresh every second at 500msec
    int timer = 1000 + (500-datetime.time().msec());
    m_timer->start(timer);
}

QTime DateTimeUtils::time(int h, int m, int s, int ms)
{
    return QTime(h, m, s, ms);
}

QDate DateTimeUtils::date(int year, int month, int day)
{
    return QDate(year, month, day);
}

QDateTime DateTimeUtils::dateTime(int year, int month, int day, int h, int m, int s, int ms)
{
    return QDateTime(QDate(year, month, day), QTime(h, m, s, ms));
}

QDateTime DateTimeUtils::dateTime(const QDate& date, const QTime& time)
{
    return QDateTime(date, time);
}

QDateTime DateTimeUtils::dateTimeAddDays(const QDateTime& dateTime, int days)
{
    return dateTime.addDays(days);
}

bool DateTimeUtils::isTimeValid(const QTime& time, const QTime& from, const QTime& to)
{
    if(!time.isValid())
        return false;
    if(!from.isValid() && !to.isValid())
        return true;
    if(!from.isValid())
        return time <= to;
    if(!to.isValid())
        return from <= time;
    return (from <= time) && (time <= to);
}

bool DateTimeUtils::isDateValid(const QDate& date, const QDate& from, const QDate& to)
{
    if(!date.isValid())
        return false;
    if(!from.isValid() && !to.isValid())
        return true;
    if(!from.isValid())
        return date <= to;
    if(!to.isValid())
        return from <= date;
    return (from <= date) && (date <= to);
}

bool DateTimeUtils::isDateTimeValid(const QDateTime& dateTime, const QDateTime& from, const QDateTime& to)
{
    if(!dateTime.isValid())
        return false;
    if(!from.isValid() && !to.isValid())
        return true;
    if(!from.isValid())
        return dateTime <= to;
    if(!to.isValid())
        return from <= dateTime;
    return (from <= dateTime) && (dateTime <= to);
}

QTime DateTimeUtils::currentTime()
{
    return QTime::currentTime();
}

QDate DateTimeUtils::currentDate()
{
    return QDate::currentDate();
}

QDateTime DateTimeUtils::currentDateTime()
{
    return QDateTime::currentDateTime();
}

QTime DateTimeUtils::invalidTime()
{
    return QTime();
}

QDate DateTimeUtils::invalidDate()
{
    return QDate();
}

QDateTime DateTimeUtils::invalidDateTime()
{
    return QDateTime();
}

QDateTime DateTimeUtils::fromSecsSinceEpoch(qint64 secs)
{
    return QDateTime::fromSecsSinceEpoch(secs);
}

int DateTimeUtils::compareTime(const QTime& left, const QTime& right)
{
    if (left==right)
        return 0;
    return (left < right) ? -1 : 1;
}

int DateTimeUtils::compareDate(const QDate& left, const QDate& right)
{
    if (left==right)
        return 0;
    return (left < right) ? -1 : 1;
}

int DateTimeUtils::compareDateTime(const QDateTime& left, const QDateTime& right)
{
    if (left==right)
        return 0;
    return (left < right) ? -1 : 1;
}

QString DateTimeUtils::dayName(int day, QLocale::FormatType format)
{
    return QLocale().dayName(day, format);
}

QString DateTimeUtils::monthName(int month, QLocale::FormatType format)
{
    return QLocale().monthName(month, format);
}

bool DateTimeUtils::isDateEqual(const QDateTime& datetime1, const QDateTime& datetime2)
{
    const QDate date1 = datetime1.date();
    const QDate date2 = datetime2.date();
    return date1.year() == date2.year() &&
           date1.month() == date2.month() &&
           date1.day() == date2.day();
}

bool DateTimeUtils::isDateToday(const QDateTime& date)
{
    const QDateTime today = QDateTime::currentDateTime();
    return isDateEqual(date, today);
}

bool DateTimeUtils::isDateTomorrow(const QDateTime& date)
{
    const QDateTime tomorrow = QDateTime::currentDateTime().addDays(1);
    return isDateEqual(date, tomorrow);
}

bool DateTimeUtils::isDateYesterday(const QDateTime& date)
{
    const QDateTime yesterday = QDateTime::currentDateTime().addDays(-1);
    return isDateEqual(date, yesterday);
}

bool DateTimeUtils::isDateBefore(const QDateTime& date1, const QDateTime& date2)
{
    return date1 < date2;
}

bool DateTimeUtils::isDateAfter(const QDateTime& date1, const QDateTime& date2)
{
    return date1 > date2;
}

QDateTime DateTimeUtils::nextValidDateTimeForTime(int h, int m, int s, int ms)
{
    QDateTime dateTime = QDateTime(QDate::currentDate(), QTime(h, m, s, ms));
    if(dateTime > QDateTime::currentDateTime())
        return dateTime;

    return dateTime.addDays(1);
}

QString DateTimeUtils::formatTime(const QTime& time, QLocale::FormatType format)
{
    return QLocale().toString(time, format);
}

QString DateTimeUtils::formatDate(const QDate& date, QLocale::FormatType format)
{
    return QLocale().toString(date, format);
}

QString DateTimeUtils::formatDateTime(const QDateTime& dateTime, QLocale::FormatType format)
{
    return QLocale().toString(dateTime, format);
}

QString DateTimeUtils::formatSecsSinceEpoch(qint64 secs, QLocale::FormatType format)
{
    return QLocale().toString(QDateTime::fromSecsSinceEpoch(secs), format);
}

enum TimeConstants {
    MSecsInDay = 86400000,
    MSecsInHour = 3600000,
    MSecsInMinute = 60000,
    MSecsInSecond = 1000,
};

QString DateTimeUtils::formatDuration(quint64 msecs, DurationFormatOptions::Flag options)
{
    quint64 ms = msecs;
    if (options & DurationFormatOptions::HideSeconds) {
        // round to nearest minute
        ms = qRound64(ms / (double)MSecsInMinute) * MSecsInMinute;
    } else if (!(options & DurationFormatOptions::ShowMilliseconds)) {
        // round to nearest second
        ms = qRound64(ms / (double)MSecsInSecond) * MSecsInSecond;
    }

    int hours = ms / MSecsInHour;
    ms = ms % MSecsInHour;
    int minutes = ms / MSecsInMinute;
    ms = ms % MSecsInMinute;
    int seconds = ms / MSecsInSecond;
    ms = ms % MSecsInSecond;

    if ((options & DurationFormatOptions::InitialDuration) == DurationFormatOptions::InitialDuration) {
        if ((options & DurationFormatOptions::FoldHours) == DurationFormatOptions::FoldHours && (options & DurationFormatOptions::ShowMilliseconds) == DurationFormatOptions::ShowMilliseconds) {
            return QString("%1m%2.%3s").arg(hours * 60 + minutes, 1, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')).arg(ms, 3, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::FoldHours) == DurationFormatOptions::FoldHours) {
            return QString("%1m%2s").arg(hours * 60 + minutes, 1, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::HideSeconds) == DurationFormatOptions::HideSeconds) {
            return QString("%1h%2m").arg(hours, 1, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::ShowMilliseconds) == DurationFormatOptions::ShowMilliseconds) {
            return QString("%1h%2m%3.%4s")
                .arg(hours, 1, 10, QLatin1Char('0'))
                .arg(minutes, 2, 10, QLatin1Char('0'))
                .arg(seconds, 2, 10, QLatin1Char('0'))
                .arg(ms, 3, 10, QLatin1Char('0'));
        } else { // Default
            return QString("%1h%2m%3s").arg(hours, 1, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
        }

    } else {
        if ((options & DurationFormatOptions::FoldHours) == DurationFormatOptions::FoldHours && (options & DurationFormatOptions::ShowMilliseconds) == DurationFormatOptions::ShowMilliseconds) {
            return QString("%1:%2.%3").arg(hours * 60 + minutes, 1, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0')).arg(ms, 3, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::FoldHours) == DurationFormatOptions::FoldHours) {
            return QString("%1:%2").arg(hours * 60 + minutes, 1, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::HideSeconds) == DurationFormatOptions::HideSeconds) {
            return QString("%1:%2").arg(hours, 1, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0'));
        } else if ((options & DurationFormatOptions::ShowMilliseconds) == DurationFormatOptions::ShowMilliseconds) {
            return QString("%1:%2:%3.%4")
                .arg(hours, 1, 10, QLatin1Char('0'))
                .arg(minutes, 2, 10, QLatin1Char('0'))
                .arg(seconds, 2, 10, QLatin1Char('0'))
                .arg(ms, 3, 10, QLatin1Char('0'));
        } else { // Default
            return QString("%1:%2:%3").arg(hours, 1, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
        }
    }

    Q_UNREACHABLE();
    return QString();
}

QString DateTimeUtils::formatDecimalDuration(quint64 msecs, int decimalPlaces)
{
    if (msecs >= MSecsInDay) {
        return tr("%1 days").arg(QLocale().toString(msecs / ((int)MSecsInDay * 1.0), 'f', decimalPlaces));
    } else if (msecs >= MSecsInHour) {
        return tr("%1 hours").arg(QLocale().toString(msecs / ((int)MSecsInHour * 1.0), 'f', decimalPlaces));
    } else if (msecs >= MSecsInMinute) {
        return tr("%1 minutes").arg(QLocale().toString(msecs / ((int)MSecsInMinute * 1.0), 'f', decimalPlaces));
    } else if (msecs >= MSecsInSecond) {
        return tr("%1 seconds").arg(QLocale().toString(msecs / ((int)MSecsInSecond * 1.0), 'f', decimalPlaces));
    }
    return tr("%n millisecond(s)").arg(msecs);
}

enum DurationUnits {
    Days = 0,
    Hours,
    Minutes,
    Seconds,
};

static QString formatSingleDuration(DurationUnits units, int n)
{
    // NB: n is guaranteed to be non-negative
    switch (units) {
    case Days:
        return DateTimeUtils::tr("%n day(s)").arg(n);
    case Hours:
        return DateTimeUtils::tr("%n hour(s)").arg(n);
    case Minutes:
        return DateTimeUtils::tr("%n minute(s)").arg(n);
    case Seconds:
        return DateTimeUtils::tr("%n second(s)").arg(n);
    }
    Q_ASSERT(false);
    return QString();
}

QString DateTimeUtils::formatSpelloutDuration(quint64 msecs)
{
    quint64 ms = msecs;
    int days = ms / MSecsInDay;
    ms = ms % (MSecsInDay);
    int hours = ms / MSecsInHour;
    ms = ms % MSecsInHour;
    int minutes = ms / MSecsInMinute;
    ms = ms % MSecsInMinute;
    int seconds = qRound(ms / 1000.0);

    // Handle correctly problematic case #1 (look at KFormatTest::prettyFormatDuration())
    if (seconds == 60) {
        return formatSpelloutDuration(msecs - ms + MSecsInMinute);
    }

    if (days && hours) {
        return tr("%1 and %2").arg(formatSingleDuration(Days, days), formatSingleDuration(Hours, hours));
    } else if (days) {
        return formatSingleDuration(Days, days);
    } else if (hours && minutes) {
        return tr("%1 and %2").arg(formatSingleDuration(Hours, hours), formatSingleDuration(Minutes, minutes));
    } else if (hours) {
        return formatSingleDuration(Hours, hours);
    } else if (minutes && seconds) {
        return tr("%1 and %2").arg(formatSingleDuration(Minutes, minutes), formatSingleDuration(Seconds, seconds));
    } else if (minutes) {
        return formatSingleDuration(Minutes, minutes);
    } else {
        return formatSingleDuration(Seconds, seconds);
    }
}

QString DateTimeUtils::formatRelativeDate(const QDate &date, QLocale::FormatType format)
{
    if (!date.isValid()) {
        return tr("Date invalide", "used when a relative date string can't be generated because the date is invalid");
    }

    const qint64 daysTo = QDate::currentDate().daysTo(date);
    if (daysTo > 2 || daysTo < -2) {
        return QLocale().toString(date, format);
    }

    QString relative;
    switch (daysTo) {
    case 2:
        relative = tr("Dans deux jours");
        break;
    case 1:
        relative = tr("Demain");
        break;
    case 0:
        relative = tr("Aujourd'hui");
        break;
    case -1:
        relative = tr("Hier");
        break;
    case -2:
        relative = tr("Il y a deux jours");
        break;
    }

    switch (format) {
    case QLocale::NarrowFormat:
        return relative;
    default:
        return QString("%1, %2").arg(relative, formatDate(date, format));
    }

    Q_UNREACHABLE();
}

QString DateTimeUtils::formatRelativeDateTime(const QDateTime &dateTime, QLocale::FormatType format)
{
    const QDateTime now = QDateTime::currentDateTime();

    const auto secsToNow = dateTime.secsTo(now);
    constexpr int secsInAHour = 60 * 60;
    if (secsToNow >= 0 && secsToNow < secsInAHour) {
        const int minutesToNow = secsToNow / 60;
        if (minutesToNow <= 1) {
            return tr("Just now");
        } else {
            return tr("%n minute(s) ago").arg(minutesToNow);
        }
    }

    const auto timeFormatType = format == QLocale::FormatType::LongFormat ? QLocale::FormatType::ShortFormat : format;
    const qint64 daysToNow = dateTime.daysTo(now);
    QString dateString;
    if (daysToNow < 2 && daysToNow > -2) {
        dateString = formatRelativeDate(dateTime.date(), format);
    } else {
        dateString = QLocale().toString(dateTime.date(), format);
    }

    QString formattedDate = tr("%1 at %2").arg(dateString, QLocale().toString(dateTime.time(), timeFormatType));

    return formattedDate.replace(0, 1, formattedDate.at(0).toUpper());
}
