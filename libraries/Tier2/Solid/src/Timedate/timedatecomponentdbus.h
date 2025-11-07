#ifndef TIMEDATECOMPONENTDBUS_H
#define TIMEDATECOMPONENTDBUS_H

#include <QtDBus>

class OrgFreedesktopTimedate1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(bool CanNTP READ canNTP CONSTANT FINAL)
    Q_PROPERTY(bool LocalRTC READ localRTC CONSTANT FINAL)
    Q_PROPERTY(bool NTP READ nTP CONSTANT FINAL)
    Q_PROPERTY(QString Timezone READ timezone CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.timedate1"; }

    OrgFreedesktopTimedate1Interface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr):
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    inline bool canNTP() const { return qvariant_cast< bool >(property("CanNTP")); }
    inline bool localRTC() const { return qvariant_cast< bool >(property("LocalRTC")); }
    inline bool nTP() const { return qvariant_cast< bool >(property("NTP")); }
    inline QString timezone() const { return qvariant_cast< QString >(property("Timezone")); }

public Q_SLOTS: // METHODS
    inline QDBusMessage SetLocalRTC(bool local_rtc, bool fix_system, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(local_rtc)<<QVariant::fromValue(fix_system)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetLocalRTC"), argumentList);
    }

    inline QDBusMessage SetNTP(bool use_ntp, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(use_ntp)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetNTP"), argumentList);
    }

    inline QDBusMessage SetTime(qlonglong usec_utc, bool relative, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(usec_utc)<<QVariant::fromValue(relative)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTime"), argumentList);
    }

    inline QDBusMessage SetTimezone(const QString &timezone, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(timezone)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTimezone"), argumentList);
    }
};

class OrgFreedesktopTimesync1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(QString ServerName READ serverName CONSTANT FINAL)
    Q_PROPERTY(QVariant ServerAddress READ serverAddress CONSTANT FINAL)
    Q_PROPERTY(qint64 Frequency READ frequency CONSTANT FINAL)
    Q_PROPERTY(quint64 PollIntervalMinUSec READ pollIntervalMinUSec CONSTANT FINAL)
    Q_PROPERTY(quint64 PollIntervalMaxUSec READ pollIntervalMaxUSec CONSTANT FINAL)
    Q_PROPERTY(quint64 PollIntervalUSec READ pollIntervalUSec CONSTANT FINAL)
    Q_PROPERTY(quint64 RootDistanceMaxUSec READ rootDistanceMaxUSec CONSTANT FINAL)
    Q_PROPERTY(QStringList FallbackNTPServers READ fallbackNTPServers CONSTANT FINAL)
    Q_PROPERTY(QStringList SystemNTPServers READ systemNTPServers CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.timesync1.Manager"; }

    OrgFreedesktopTimesync1Interface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr):
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    inline QString serverName() const { return qvariant_cast<QString>(property("ServerName")); }
    inline QVariant serverAddress() const { return property("ServerAddress"); }  // (iay)
    inline qint64 frequency() const { return qvariant_cast<qint64>(property("Frequency")); }
    inline quint64 pollIntervalMinUSec() const { return qvariant_cast<quint64>(property("PollIntervalMinUSec")); }
    inline quint64 pollIntervalMaxUSec() const { return qvariant_cast<quint64>(property("PollIntervalMaxUSec")); }
    inline quint64 pollIntervalUSec() const { return qvariant_cast<quint64>(property("PollIntervalUSec")); }
    inline quint64 rootDistanceMaxUSec() const { return qvariant_cast<quint64>(property("RootDistanceMaxUSec")); }
    inline QStringList fallbackNTPServers() const { return qvariant_cast<QStringList>(property("FallbackNTPServers")); }
    inline QStringList systemNTPServers() const { return qvariant_cast<QStringList>(property("SystemNTPServers")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> SetRuntimeNTPServers(const QStringList &servers, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(servers)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetRuntimeNTPServers"), argumentList);
    }
};

namespace org {
    namespace freedesktop {
        typedef ::OrgFreedesktopTimedate1Interface timedate1;
        typedef ::OrgFreedesktopTimesync1Interface timesync1;
    }
}

#include "abstracttimedatecomponent.h"
class TimedateComponentDBus : public AbstractTimedateComponent
{
public:
    TimedateComponentDBus();

    int getCapabilities() override;

    QString getTimezone() const override;
    bool setTimezone(const QString& aTimeZone) override;

    bool getNtp() const override;
    bool setNtp(bool aNtp) override;

    bool setSystemTime(const QDateTime& aTime) override;

    QString getNtpServer() const override;
    bool setNtpServer(const QString& ntpServer) override;

    QString getServerName() const override;

private:
    OrgFreedesktopTimedate1Interface *m_timedateInterface;
    OrgFreedesktopTimesync1Interface *m_timesyncInterface;
};

#endif // TIMEDATECOMPONENTDBUS_H
