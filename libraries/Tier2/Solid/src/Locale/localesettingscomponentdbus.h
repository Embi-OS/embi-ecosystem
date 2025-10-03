#ifndef LOCALESETTINGSCOMPONENTDBUS_H
#define LOCALESETTINGSCOMPONENTDBUS_H

#include <QtDBus>

class OrgFreedesktopLocale1Interface : public QDBusAbstractInterface
{
    Q_OBJECT
    Q_PROPERTY(QStringList Locale READ locale CONSTANT FINAL)

public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.locale1"; }

    OrgFreedesktopLocale1Interface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr)
        : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {}

    inline QStringList locale() const { return qvariant_cast< QStringList >(property("Locale")); }

public Q_SLOTS: // METHODS
    inline QDBusMessage SetLocale(const QStringList &locale, bool user_interaction) {
        QVariantList argumentList;
        argumentList<<QVariant::fromValue(locale)<<QVariant::fromValue(user_interaction);
        return callWithArgumentList(QDBus::Block, QStringLiteral("SetLocale"), argumentList);
    }
};

namespace org {
  namespace freedesktop {
    typedef ::OrgFreedesktopLocale1Interface locale1;
  }
}

#include "abstractlocalecomponent.h"
class LocaleSettingsComponentDBus : public AbstractLocaleComponent
{
public:
    LocaleSettingsComponentDBus();

    int getCapabilities() override;

    QString getLocale() const override;
    bool setLocale(const QString& locale) override;

private:
    void updateLocale();

    OrgFreedesktopLocale1Interface *m_interface;
    QHash<QString, QString> m_localeCache;
};

#endif // LOCALESETTINGSCOMPONENTDBUS_H
