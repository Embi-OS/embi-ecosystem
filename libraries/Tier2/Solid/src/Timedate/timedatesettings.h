#ifndef TIMEDATESETTINGS_H
#define TIMEDATESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

class TimedateSettings : public QObject,
                         public QQmlSingleton<TimedateSettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canSetTimezone          READ canSetTimezone         CONSTANT FINAL)
    Q_PROPERTY(bool canSetNtp               READ canSetNtp              CONSTANT FINAL)
    Q_PROPERTY(bool canSetSystemDateTime    READ canSetSystemDateTime   CONSTANT FINAL)
    Q_PROPERTY(bool canSetNtpServer         READ canSetNtpServer        CONSTANT FINAL)
    Q_PROPERTY(bool canReadRTC              READ canReadRTC             CONSTANT FINAL)
    Q_PROPERTY(QString timezone             READ getTimezone            WRITE setTimezone       NOTIFY timezoneChanged FINAL)
    Q_PROPERTY(bool ntp                     READ getNtp                 WRITE setNtp            NOTIFY ntpChanged FINAL)
    Q_PROPERTY(QDateTime systemDateTime     READ getSystemDateTime      WRITE setSystemDateTime NOTIFY systemDateTimeChanged FINAL)
    Q_PROPERTY(QDate systemDate             READ getSystemDate          WRITE setSystemDate     NOTIFY systemDateTimeChanged FINAL)
    Q_PROPERTY(QTime systemTime             READ getSystemTime          WRITE setSystemTime     NOTIFY systemDateTimeChanged FINAL)
    Q_PROPERTY(QString ntpServer            READ getNtpServer           WRITE setNtpServer      NOTIFY ntpServerChanged FINAL)
    Q_PROPERTY(QString serverName           READ getServerName          NOTIFY timesyncChanged FINAL)

protected:
    friend QQmlSingleton<TimedateSettings>;
    explicit TimedateSettings(QObject *parent = nullptr);

public:
    static bool canSetTimezone();
    static bool canSetNtp();
    static bool canSetSystemDateTime();
    static bool canSetNtpServer();
    static bool canReadRTC();

    QString getTimezone() const;
    bool getNtp() const;
    QDateTime getSystemDateTime() const;
    QDate getSystemDate() const;
    QTime getSystemTime() const;
    QString getNtpServer() const;
    QString getServerName() const;

    Q_INVOKABLE static QString timedateCtl();
    Q_INVOKABLE static QString timesyncStatus();

public slots:
    void setTimezone(const QString& timezone);
    void setNtp(const bool ntp);
    void setSystemDateTime(const QDateTime& systemDateTime);
    void setSystemDate(const QDate& systemDate);
    void setSystemTime(const QTime& systemTime);
    void setNtpServer(const QString& ntpServer);

    bool syncNtp();
    bool syncRtc();

signals:
    void timezoneChanged();
    void ntpChanged();
    void systemDateTimeChanged();
    void timeserversChanged();
    void ntpServerChanged();
    void timesyncChanged();

private slots:
    void refreshDateTime();
    void updateSystemDateTime();

private:
    QTimer* m_timer=nullptr;
    QTimer* m_systemDateTimeCaller=nullptr;
    QDate m_systemDateTemp;
    QTime m_systemTimeTemp;
};

#endif // TIMEDATESETTINGS_H
