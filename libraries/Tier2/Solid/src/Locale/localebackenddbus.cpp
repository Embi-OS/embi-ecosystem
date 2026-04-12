#include "localebackenddbus.h"

#include "locale1interface.h"

LocaleBackendDbus::LocaleBackendDbus(QObject* parent):
    LocaleBackend(parent),
    m_interface(new OrgFreedesktopLocale1Interface(QStringLiteral("org.freedesktop.locale1"),
                                                   QStringLiteral("/org/freedesktop/locale1"),
                                                   QDBusConnection::systemBus(), this))
{
    const QStringList localeList = m_interface->locale();
    for (const QString &entry : localeList) {
        const QStringList nameValue = entry.split('=');
        if (nameValue.length() == 2)
            m_localeCache[nameValue.at(0)] = nameValue.at(1);
    }
}

int LocaleBackendDbus::getCapabilities()
{
    return Capabilities::Locale;
}

QString LocaleBackendDbus::getLocale() const
{
    return m_localeCache.value(QStringLiteral("LANG"), LocaleBackend::getLocale());
}

bool LocaleBackendDbus::setLocale(const QString &localeValue)
{
    if (m_localeCache.value(QStringLiteral("LANG")) == localeValue)
        return false;

    m_localeCache[QStringLiteral("LANG")] = localeValue;
    m_localeCache[QStringLiteral("LANGUAGE")] = localeValue;
    updateLocale();
    return true;
}

void LocaleBackendDbus::updateLocale()
{
    QStringList newLocale;
    for (auto it = m_localeCache.cbegin(); it != m_localeCache.cend(); ++it)
        newLocale.append(it.key() + QLatin1Char('=') + it.value());

    m_interface->SetLocale(newLocale, true);
}
