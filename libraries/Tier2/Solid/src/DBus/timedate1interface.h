#ifndef TIMEDATE1INTERFACE_H
#define TIMEDATE1INTERFACE_H

#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include <QString>
#include <QVariant>

class OrgFreedesktopTimedate1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(bool CanNTP READ canNTP CONSTANT FINAL)
    Q_PROPERTY(bool LocalRTC READ localRTC CONSTANT FINAL)
    Q_PROPERTY(bool NTP READ nTP CONSTANT FINAL)
    Q_PROPERTY(QString Timezone READ timezone CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    {
        return "org.freedesktop.timedate1";
    }

    OrgFreedesktopTimedate1Interface(const QString &service,
                                     const QString &path,
                                     const QDBusConnection &connection,
                                     QObject *parent = nullptr) :
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
    }

    inline bool canNTP() const { return qvariant_cast<bool>(property("CanNTP")); }
    inline bool localRTC() const { return qvariant_cast<bool>(property("LocalRTC")); }
    inline bool nTP() const { return qvariant_cast<bool>(property("NTP")); }
    inline QString timezone() const { return qvariant_cast<QString>(property("Timezone")); }

public Q_SLOTS:
    inline QDBusMessage SetLocalRTC(bool localRtc, bool fixSystem, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(localRtc),
                                QVariant::fromValue(fixSystem),
                                QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetLocalRTC"), args);
    }

    inline QDBusMessage SetNTP(bool useNtp, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(useNtp), QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetNTP"), args);
    }

    inline QDBusMessage SetTime(qlonglong usecUtc, bool relative, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(usecUtc),
                                QVariant::fromValue(relative),
                                QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTime"), args);
    }

    inline QDBusMessage SetTimezone(const QString &timezoneValue, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(timezoneValue), QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetTimezone"), args);
    }
};

namespace org::freedesktop
{
typedef ::OrgFreedesktopTimedate1Interface timedate1;
}

#endif // TIMEDATE1INTERFACE_H
