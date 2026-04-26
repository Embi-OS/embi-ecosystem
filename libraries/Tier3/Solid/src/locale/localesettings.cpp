#include "localesettings.h"
#include "solid_log.h"

#include "qtranslatorloader.h"

#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
#include "locale1interface.h"
#endif

LocaleSettings::LocaleSettings(QObject *parent) :
    QObject(parent),
#if !defined(QT_NO_DBUS) && (defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX))
    m_interface(new OrgFreedesktopLocale1Interface(QStringLiteral("org.freedesktop.locale1"),
                                                   QStringLiteral("/org/freedesktop/locale1"),
                                                   QDBusConnection::systemBus(), this)),
#endif
    m_translator(new QTranslatorLoader(this))
{
    if (!m_interface) {
        SOLIDLOG_WARNING() << "Could not find a localesettings backend matching this platform";
    }

    m_translator->componentComplete();
    QLocale::setDefault(QLocale(getLocale()));

    reloadTranslator();
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
    return reloadTranslator();
}

bool LocaleSettings::reloadTranslator()
{
    m_translator->setLanguage(QLocale(getLocale()).name().split("_").at(0));
    return true;
}

void LocaleSettings::updateLocale()
{
    QStringList newLocale;
    for (auto it = m_localeCache.cbegin(); it != m_localeCache.cend(); ++it)
        newLocale.append(it.key() + QLatin1Char('=') + it.value());

    m_interface->SetLocale(newLocale, true);
}

