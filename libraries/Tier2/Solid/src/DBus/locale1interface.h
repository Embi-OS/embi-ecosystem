#ifndef LOCALE1INTERFACE_H
#define LOCALE1INTERFACE_H

#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include <QString>
#include <QStringList>
#include <QVariant>

class OrgFreedesktopLocale1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(QStringList Locale READ locale CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    {
        return "org.freedesktop.locale1";
    }

    OrgFreedesktopLocale1Interface(const QString &service,
                                   const QString &path,
                                   const QDBusConnection &connection,
                                   QObject *parent = nullptr) :
        QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
    }

    inline QStringList locale() const { return qvariant_cast<QStringList>(property("Locale")); }

public Q_SLOTS:
    inline QDBusMessage SetLocale(const QStringList &locale, bool userInteraction)
    {
        const QVariantList args{QVariant::fromValue(locale), QVariant::fromValue(userInteraction)};
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetLocale"), args);
    }
};

namespace org::freedesktop
{
typedef ::OrgFreedesktopLocale1Interface locale1;
}

#endif // LOCALE1INTERFACE_H
