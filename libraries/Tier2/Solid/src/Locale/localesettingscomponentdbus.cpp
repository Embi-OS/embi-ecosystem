#include "localesettingscomponentdbus.h"

LocaleSettingsComponentDBus::LocaleSettingsComponentDBus()
{
    m_interface = new OrgFreedesktopLocale1Interface(QStringLiteral("org.freedesktop.locale1"),
                                                     QStringLiteral("/org/freedesktop/locale1"),
                                                     QDBusConnection::systemBus());

    const QStringList locale = m_interface->locale();
    for (const QString& l: locale) {
        QStringList nameValue = l.split('=');
        if (nameValue.length() == 2)
            m_localeCache[nameValue.at(0)] = nameValue.at(1);
    }
}

int LocaleSettingsComponentDBus::getCapabilities()
{
    return Capabilities::Locale;
}

QString LocaleSettingsComponentDBus::getLocale() const
{
    if(!m_interface)
        return QString();

    return m_localeCache[QStringLiteral("LANG")];
}

bool LocaleSettingsComponentDBus::setLocale(const QString& locale)
{
    if(!m_interface)
        return false;

    if(m_localeCache[QStringLiteral("LANG")]==locale)
        return false;
    m_localeCache[QStringLiteral("LANG")] = locale;
    m_localeCache[QStringLiteral("LANGUAGE")] = locale;
    updateLocale();
    return true;
}

void LocaleSettingsComponentDBus::updateLocale() {
    QStringList newLocale;
    QHash<QString, QString>::iterator i;
    for (i = m_localeCache.begin(); i != m_localeCache.end(); ++i) {
        QString val = i.key() + QLatin1String("=") + i.value();
        newLocale.append(val);
    }
    m_interface->SetLocale(newLocale, true);
}
