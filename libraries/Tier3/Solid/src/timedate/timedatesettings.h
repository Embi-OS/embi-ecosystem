#ifndef TIMEDATESETTINGS_H
#define TIMEDATESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

class OrgFreedesktopTimedate1Interface;
class OrgFreedesktopTimesync1Interface;
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
    Q_PROPERTY(bool canSyncRTC              READ canSyncRTC             CONSTANT FINAL)
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
    enum Capabilities
    {
        Timezone        = ( 1<<0 ),
        Ntp             = ( 1<<1 ),
        SystemDateTime  = ( 1<<2 ),
        NtpServer       = ( 1<<3 ),
    };

    int getCapabilities() const;

    bool hasCapability(int capability) const { return getCapabilities() & capability; };
    bool canSetTimezone() const { return hasCapability(Capabilities::Timezone); }
    bool canSetNtp() const { return hasCapability(Capabilities::Ntp); }
    bool canSetSystemDateTime() const { return hasCapability(Capabilities::SystemDateTime); }
    bool canSetNtpServer() const { return hasCapability(Capabilities::NtpServer); }
    bool canSyncRTC() const;

    QString getTimezone() const;
    bool getNtp() const;
    QDateTime getSystemDateTime() const;
    QDate getSystemDate() const;
    QTime getSystemTime() const;
    QString getNtpServer() const;
    QString getServerName() const;

    static QString hwclock(const QStringList& arguments=QStringList(), bool* ok=nullptr);

public slots:
    bool setTimezone(const QString& timezone);
    bool setNtp(bool ntp);
    bool setSystemDateTime(const QDateTime& systemDateTime);
    bool setSystemDate(const QDate& systemDate);
    bool setSystemTime(const QTime& systemTime);
    bool setNtpServer(const QString& ntpServer);

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
    OrgFreedesktopTimedate1Interface* m_timedateInterface=nullptr;
    OrgFreedesktopTimesync1Interface* m_timesyncInterface=nullptr;

    QTimer* m_timer=nullptr;
    QTimer* m_systemDateTimeCaller=nullptr;
    QDate m_systemDateTemp;
    QTime m_systemTimeTemp;
};

#endif // TIMEDATESETTINGS_H
