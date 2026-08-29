#include "languagesettings.h"
#include "axion_log.h"

#include "qtranslatorloader.h"
#include "qsettingsmapper.h"

#include <QLocale>
#include <QScopedPointer>
#include <QSettings>

LanguageSettings::LanguageSettings(QObject *parent) :
    QObject(parent),
    m_translator(new QTranslatorLoader(this))
{
    m_translator->componentComplete();
    connect(m_translator, &QTranslatorLoader::translationReloaded, this, [this]() {
        if(m_qmlEngine)
            m_qmlEngine->retranslate();
    });

    readSettings();
    updateEffectiveLanguage();
}

void LanguageSettings::init(QQmlEngine* engine)
{
    LanguageSettings::Get()->setEngine(engine);
}

QString LanguageSettings::systemLanguageName()
{
    return QStringLiteral("system");
}

const QString& LanguageSettings::language() const
{
    return m_language;
}

const QString& LanguageSettings::effectiveLanguage() const
{
    return m_effectiveLanguage;
}

void LanguageSettings::setEngine(QQmlEngine* engine)
{
    m_qmlEngine = engine;
}

bool LanguageSettings::setLanguage(const QString& language)
{
    return setLanguage(language, true);
}

bool LanguageSettings::setLanguage(const QString& language, bool persist)
{
    const QString normalized = normalizedLanguage(language);
    if(normalized.isEmpty())
        return false;

    if(m_language == normalized)
        return false;

    m_language = normalized;
    emit languageChanged(m_language);

    if(persist)
        writeSettings();

    return updateEffectiveLanguage();
}

bool LanguageSettings::resetToSystemLanguage()
{
    return setLanguage(systemLanguageName());
}

void LanguageSettings::refreshSystemLanguage()
{
    if(isSystemLanguage())
        updateEffectiveLanguage();
}

bool LanguageSettings::reloadTranslations()
{
    return m_translator->setLanguage(m_effectiveLanguage);
}

QString LanguageSettings::systemLanguage() const
{
    const QStringList uiLanguages = QLocale().uiLanguages();
    if(!uiLanguages.isEmpty())
        return normalizedLanguage(uiLanguages.first());

    return normalizedLanguage(QLocale().name());
}

QString LanguageSettings::normalizedLanguage(const QString& language) const
{
    if(language.compare(systemLanguageName(), Qt::CaseInsensitive) == 0)
        return systemLanguageName();

    return QTranslatorLoader::normalizedLanguage(language);
}

bool LanguageSettings::isSystemLanguage() const
{
    return m_language == systemLanguageName();
}

void LanguageSettings::readSettings()
{
    QScopedPointer<QSettings> settings(QSettingsMapper::createSettings("language.conf"));

    const QString storedLanguage = settings->value("language").toString();

    if(storedLanguage.isEmpty())
        m_language = systemLanguageName();
    else
        m_language = normalizedLanguage(storedLanguage);

    if(m_language.isEmpty())
        m_language = systemLanguageName();
}

void LanguageSettings::writeSettings()
{
    QScopedPointer<QSettings> settings(QSettingsMapper::createSettings("language.conf"));
    settings->setValue("language", m_language);
}

bool LanguageSettings::updateEffectiveLanguage()
{
    const QString nextLanguage = isSystemLanguage() ? systemLanguage() : m_language;
    if(nextLanguage.isEmpty())
        return false;

    const bool changed = m_effectiveLanguage != nextLanguage;
    m_effectiveLanguage = nextLanguage;

    const bool reloaded = reloadTranslations();
    if(changed)
        emit effectiveLanguageChanged(m_effectiveLanguage);

    AXIONLOG_INFO()<<"Application language:"<<m_effectiveLanguage;

    return changed || reloaded;
}
