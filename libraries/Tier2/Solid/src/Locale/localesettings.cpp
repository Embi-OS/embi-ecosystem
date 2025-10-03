#include "localesettings.h"
#include "solid_log.h"

#include "qtranslatorloader.h"

#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
#include "localesettingscomponentdbus.h"
#endif
#include "abstractlocalecomponent.h"

static AbstractLocaleComponent* getComponent()
{
    static AbstractLocaleComponent* instance = nullptr;

    if(!instance)
    {
#if defined(Q_OS_BOOT2QT) || defined(LINUX_DBUS) || defined(Q_OS_LINUX)
        instance = new LocaleSettingsComponentDBus();
#endif
    }

    return instance;
}

LocaleSettings::LocaleSettings(QObject *parent) :
    QObject(parent),
    m_translator(new QTranslatorLoader(this))
{
    if(!getComponent()) {
        SOLIDLOG_WARNING()<<"Could not find a localesettings component matching this platform";
    }

    m_translator->componentComplete();
    QLocale::setDefault(QLocale(getLocale()));

    reloadTranslator();
}

void LocaleSettings::init()
{
    LocaleSettings::Get();
}

bool LocaleSettings::canSetLocale()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetLocale();
}

QString LocaleSettings::getLocale() const
{
    if(!canSetLocale())
    {
        SOLIDLOG_DEBUG()<<"Cannot get locale, fallback to default";
        return QLocale::system().name();
    }

    return QLocale(getComponent()->getLocale()).name();
}

bool LocaleSettings::setLocale(const QString& locale)
{
    if(!canSetLocale())
    {
        SOLIDLOG_WARNING()<<"Cannot set locale";
        return false;
    }

    if(!getComponent()->setLocale(locale))
        return false;

    QLocale::setDefault(QLocale(locale));
    emit this->localeChanged();
    return reloadTranslator();
}

bool LocaleSettings::reloadTranslator()
{
    m_translator->setLanguage(QLocale(getLocale()).name().split("_").at(0));
    return true;
}
