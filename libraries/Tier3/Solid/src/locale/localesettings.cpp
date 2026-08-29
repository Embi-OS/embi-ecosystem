#include "localesettings.h"
#include "solid_log.h"

#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
#include "locale1interface.h"
#endif

#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
LocaleSettings::LocaleSettings(QObject *parent) :
    QObject(parent),
    m_interface(new OrgFreedesktopLocale1Interface(QStringLiteral("org.freedesktop.locale1"),
                                                   QStringLiteral("/org/freedesktop/locale1"),
                                                   QDBusConnection::systemBus(), this))
#else
LocaleSettings::LocaleSettings(QObject *parent) :
    QObject(parent)
#endif
{
    if (!m_interface) {
        SOLIDLOG_WARNING() << "Could not find a localesettings backend matching this platform";
    }

    QLocale::setDefault(QLocale(getLocale()));
}

void LocaleSettings::init()
{
    LocaleSettings::Get();
}

int LocaleSettings::getCapabilities() const
{
    if(!m_interface)
        return 0;
    return Capabilities::Locale;
}

QString LocaleSettings::getLocale() const
{
    if(!m_interface)
    {
        SOLIDLOG_DEBUG()<<"Cannot get locale, fallback to default";
        return QLocale::system().name();
    }

    return QLocale(m_localeCache.value(QStringLiteral("LANG"), QLocale::system().name())).name();
}

bool LocaleSettings::setLocale(const QString& locale)
{
    if(!canSetLocale())
    {
        SOLIDLOG_WARNING()<<"Cannot set locale";
        return false;
    }

    if (m_localeCache.value(QStringLiteral("LANG")) == locale)
        return false;

    m_localeCache[QStringLiteral("LANG")] = locale;
    m_localeCache[QStringLiteral("LANGUAGE")] = locale;
    updateLocale();

    QLocale::setDefault(QLocale(locale));
    emit this->localeChanged();
    return true;
}

void LocaleSettings::updateLocale()
{
    QStringList newLocale;
    for (auto it = m_localeCache.cbegin(); it != m_localeCache.cend(); ++it)
        newLocale.append(it.key() + QLatin1Char('=') + it.value());

    m_interface->SetLocale(newLocale, true);
}
