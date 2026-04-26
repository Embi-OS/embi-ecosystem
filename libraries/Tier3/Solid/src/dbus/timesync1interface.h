#ifndef TIMESYNC1INTERFACE_H
#define TIMESYNC1INTERFACE_H

#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingReply>

#include <QString>
#include <QStringList>
#include <QVariant>

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
    {
        return "org.freedesktop.timesync1.Manager";
    }

    OrgFreedesktopTimesync1Interface(const QString &service,
                                     const QString &path,
                                     const QDBusConnection &connection,
                                     QObject *parent = nullptr) :
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
    }

    inline QString serverName() const { return qvariant_cast<QString>(property("ServerName")); }
    inline QVariant serverAddress() const { return property("ServerAddress"); }
    inline qint64 frequency() const { return qvariant_cast<qint64>(property("Frequency")); }
    inline quint64 pollIntervalMinUSec() const { return qvariant_cast<quint64>(property("PollIntervalMinUSec")); }
    inline quint64 pollIntervalMaxUSec() const { return qvariant_cast<quint64>(property("PollIntervalMaxUSec")); }
    inline quint64 pollIntervalUSec() const { return qvariant_cast<quint64>(property("PollIntervalUSec")); }
    inline quint64 rootDistanceMaxUSec() const { return qvariant_cast<quint64>(property("RootDistanceMaxUSec")); }
    inline QStringList fallbackNTPServers() const { return qvariant_cast<QStringList>(property("FallbackNTPServers")); }
    inline QStringList systemNTPServers() const { return qvariant_cast<QStringList>(property("SystemNTPServers")); }

public Q_SLOTS:
    inline QDBusPendingReply<> SetRuntimeNTPServers(const QStringList &servers, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(servers), QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetRuntimeNTPServers"), args);
    }
};

namespace org::freedesktop
{
typedef ::OrgFreedesktopTimesync1Interface timesync1;
}

#endif // TIMESYNC1INTERFACE_H
