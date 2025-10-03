#ifndef DATETIMEUTILS_H
#define DATETIMEUTILS_H

#include <QDefs>
#include "qsingleton.h"

Q_FLAG_CLASS(DurationFormatOptions, DurationFormatOption, DurationFormatOptionEnum,
             DefaultDuration = 0x0, ///< Default formatting in localized 1:23:45 format
             InitialDuration = 0x1, ///< Default formatting in localized 1h23m45s format
             ShowMilliseconds = 0x2, ///< Include milliseconds in format, e.g. 1:23:45.678
             HideSeconds = 0x4, ///< Hide the seconds, e.g. 1:23 or 1h23m, overrides ShowMilliseconds
             FoldHours = 0x8,) ///< Fold the hours into the minutes, e.g. 83:45 or 83m45s, overrides HideSeconds

class DateTimeUtils : public QObject,
                      public QQmlSingleton<DateTimeUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QDateTime systemDateTime     READ getSystemDateTime      NOTIFY systemDateTimeChanged FINAL)
    Q_PROPERTY(QDate systemDate             READ getSystemDate          NOTIFY systemDateTimeChanged FINAL)
    Q_PROPERTY(QTime systemTime             READ getSystemTime          NOTIFY systemDateTimeChanged FINAL)

    Q_READONLY_VAR_PROPERTY(int, year, Year, 0)
    Q_READONLY_VAR_PROPERTY(int, month, Month, 0)
    Q_READONLY_VAR_PROPERTY(int, day, Day, 0)
    Q_READONLY_VAR_PROPERTY(int, hour, Hour, 0)
    Q_READONLY_VAR_PROPERTY(int, minute, Minute, 0)
    Q_READONLY_VAR_PROPERTY(int, second, Second, 0)

protected:
    friend QQmlSingleton<DateTimeUtils>;
    explicit DateTimeUtils(QObject *parent = nullptr);

public:
    QDateTime getSystemDateTime() const;
    QDate getSystemDate() const;
    QTime getSystemTime() const;

signals:
    void systemDateTimeChanged();

private slots:
    void refreshDateTime();

private:
    QTimer* m_timer=nullptr;

public:
    Q_INVOKABLE static QTime time(int h, int m, int s = 0, int ms = 0);
    Q_INVOKABLE static QDate date(int year, int month, int day);
    Q_INVOKABLE static QDateTime dateTime(int year, int month, int day, int h = 0, int m = 0, int s = 0, int ms = 0);
    Q_INVOKABLE static QDateTime dateTime(const QDate& date, const QTime& time=QTime());

    Q_INVOKABLE static QDateTime dateTimeAddDays(const QDateTime& dateTime, int days);

    Q_INVOKABLE static bool isTimeValid(const QTime& time, const QTime& from=QTime(), const QTime& to=QTime());
    Q_INVOKABLE static bool isDateValid(const QDate& date, const QDate& from=QDate(), const QDate& to=QDate());
    Q_INVOKABLE static bool isDateTimeValid(const QDateTime& dateTime, const QDateTime& from=QDateTime(), const QDateTime& to=QDateTime());
    Q_INVOKABLE static QTime currentTime();
    Q_INVOKABLE static QDate currentDate();
    Q_INVOKABLE static QDateTime currentDateTime();
    Q_INVOKABLE static QTime invalidTime();
    Q_INVOKABLE static QDate invalidDate();
    Q_INVOKABLE static QDateTime invalidDateTime();
    Q_INVOKABLE static QDateTime fromSecsSinceEpoch(qint64 secs);
    Q_INVOKABLE static int compareTime(const QTime& left, const QTime& right);
    Q_INVOKABLE static int compareDate(const QDate& left, const QDate& right);
    Q_INVOKABLE static int compareDateTime(const QDateTime& left, const QDateTime& right);
    Q_INVOKABLE static QString dayName(int day, QLocale::FormatType format=QLocale::LongFormat);
    Q_INVOKABLE static QString monthName(int month, QLocale::FormatType format=QLocale::LongFormat);

    Q_INVOKABLE static bool isDateEqual(const QDateTime& datetime1, const QDateTime& datetime2);
    Q_INVOKABLE static bool isDateToday(const QDateTime& date);
    Q_INVOKABLE static bool isDateTomorrow(const QDateTime& date);
    Q_INVOKABLE static bool isDateYesterday(const QDateTime& date);
    Q_INVOKABLE static bool isDateBefore(const QDateTime& date1, const QDateTime& date2);
    Q_INVOKABLE static bool isDateAfter(const QDateTime& date1, const QDateTime& date2);

    Q_INVOKABLE static QDateTime nextValidDateTimeForTime(int h, int m, int s = 0, int ms = 0);
    Q_INVOKABLE static QString formatTime(const QTime& time, QLocale::FormatType format=QLocale::LongFormat);
    Q_INVOKABLE static QString formatDate(const QDate& date, QLocale::FormatType format=QLocale::LongFormat);
    Q_INVOKABLE static QString formatDateTime(const QDateTime& dateTime, QLocale::FormatType format=QLocale::LongFormat);
    Q_INVOKABLE static QString formatSecsSinceEpoch(qint64 secs, QLocale::FormatType format=QLocale::LongFormat);
    Q_INVOKABLE static QString formatDuration(quint64 msecs, DurationFormatOptions::Flag options = DurationFormatOptions::DefaultDuration);
    Q_INVOKABLE static QString formatDecimalDuration(quint64 msecs, int decimalPlaces = 2);
    Q_INVOKABLE static QString formatSpelloutDuration(quint64 msecs);
    Q_INVOKABLE static QString formatRelativeDate(const QDate &date, QLocale::FormatType format);
    Q_INVOKABLE static QString formatRelativeDateTime(const QDateTime &dateTime, QLocale::FormatType format);
};

#endif // DATETIMEUTILS_H
