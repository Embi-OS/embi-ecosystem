#include "localesettings.h"
#include "localebackend.h"
#include "solid_log.h"

#include "qtranslatorloader.h"
#include "localebackenddbus.h"

LocaleSettings::LocaleSettings(QObject *parent) :
    QObject(parent),
#if defined(Q_OS_BOOT2QT) || defined(Q_OS_LINUX)
    m_backend(new LocaleBackendDbus(this)),
#else
    m_backend(nullptr),
#endif
    m_translator(new QTranslatorLoader(this))
{
    if (!m_backend) {
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

bool LocaleSettings::canSetLocale() const
{
    return m_backend && m_backend->canSetLocale();
}

QString LocaleSettings::getLocale() const
{
    if(!m_backend)
    {
        SOLIDLOG_DEBUG()<<"Cannot get locale, fallback to default";
        return QLocale::system().name();
    }

    return QLocale(m_backend->getLocale()).name();
}

bool LocaleSettings::setLocale(const QString& locale)
{
    if(!canSetLocale())
    {
        SOLIDLOG_WARNING()<<"Cannot set locale";
        return false;
    }

    if(!m_backend->setLocale(locale))
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
