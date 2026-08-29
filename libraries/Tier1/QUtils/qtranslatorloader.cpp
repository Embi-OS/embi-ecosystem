#include "qtranslatorloader.h"
#include "qutils_log.h"

#include <QDirIterator>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QLocale>
#include <QRegularExpression>
#include <QSet>

Q_GLOBAL_STATIC_WITH_ARGS(QString, trFilePrefix, (":/i18n"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, trFileSuffix, ("_%1.qm"))

QTranslatorLoader::QTranslatorLoader(QObject* parent) :
    QObject(parent)
{
    connect(this, &QTranslatorLoader::languageChanged, this, &QTranslatorLoader::onLanguageChanged);
}

QTranslatorLoader::~QTranslatorLoader()
{
    clearTranslations();
}

void QTranslatorLoader::classBegin()
{

}
void QTranslatorLoader::componentComplete()
{
    m_completed=true;

    loadTranslations();
}

void QTranslatorLoader::onLanguageChanged()
{
    if(loadTranslations())
        emit this->translationReloaded();
}

bool QTranslatorLoader::loadTranslations()
{
    if(!m_completed)
        return false;

    const QString language = normalizedLanguage(getLanguage());
    const bool hadTranslations = !m_translators.isEmpty();
    if(language.isEmpty() || language == QStringLiteral("c"))
        return clearTranslations() || hadTranslations;

    QList<QPair<QString, QTranslator*>> loadedTranslators;
    const QStringList fileNames = getFileNames(language);
    for(const QString& fileName: fileNames)
    {
        QString name = fileName;
        name.remove(trFileSuffix->arg(language));

        QTranslator* translator = new QTranslator(this);
        if(!QFile::exists(*trFilePrefix+"/"+fileName) || !translator->load(fileName, *trFilePrefix)) {
            delete translator;
            QUTILSLOG_WARNING()<<"Failed to load"<<fileName<<"translation file, falling back to default";
            continue;
        }

        loadedTranslators.append(qMakePair(name, translator));
    }

    clearTranslations();

    for(const auto& loadedTranslator: loadedTranslators)
    {
        m_translators[loadedTranslator.first] = loadedTranslator.second;
        qApp->installTranslator(loadedTranslator.second);
        QUTILSLOG_DEBUG()<<"Installed"<<loadedTranslator.second<<loadedTranslator.first;
    }

    QUTILSLOG_INFO()<<"Load translation files:"<<fileNames;

    return !loadedTranslators.isEmpty() || hadTranslations;
}

bool QTranslatorLoader::loadFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return false;

    const QString language = normalizedLanguage(getLanguage());
    if(!QFile::exists(*trFilePrefix+"/"+fileName))
        return false;

    QString name = fileName;
    name.remove(trFileSuffix->arg(language));
    QTranslator* translator = m_translators.value(name);
    const bool installTranslator = !translator;
    if(installTranslator)
        translator = new QTranslator(this);

    if(!translator->load(fileName, *trFilePrefix))
    {
        if(installTranslator)
            delete translator;
        return false;
    }

    if(installTranslator)
    {
        m_translators[name] = translator;
        qApp->installTranslator(translator);
        QUTILSLOG_DEBUG()<<"Installed"<<translator<<name;
    }

    return true;
}

const QStringList QTranslatorLoader::getFileNames(const QString langage) const
{
    const QString language = normalizedLanguage(langage);
    QDir dir(*trFilePrefix,"*"+trFileSuffix->arg(language));
    return dir.entryList();
}

QStringList QTranslatorLoader::availableLanguages()
{
    QSet<QString> languages;
    languages.insert(QStringLiteral("c"));

    QDir dir(*trFilePrefix, QStringLiteral("*.qm"));
    static const QRegularExpression languageExpression(QStringLiteral("_([A-Za-z]{1,8})(?:_[A-Za-z0-9]+)?\\.qm$"));
    const QStringList fileNames = dir.entryList();
    for(const QString& fileName: fileNames)
    {
        const QRegularExpressionMatch match = languageExpression.match(fileName);
        if(match.hasMatch())
            languages.insert(normalizedLanguage(match.captured(1)));
    }

    QStringList result = languages.values();
    result.sort();
    return result;
}

QString QTranslatorLoader::normalizedLanguage(const QString& language)
{
    QString normalized = QLocale(language).name();
    if(normalized.isEmpty() || normalized == QStringLiteral("C"))
        normalized = language;

    normalized.replace(QLatin1Char('-'), QLatin1Char('_'));
    normalized = normalized.section(QLatin1Char('_'), 0, 0).toLower();

    return normalized;
}

bool QTranslatorLoader::clearTranslations()
{
    if(m_translators.isEmpty())
        return false;

    const QList<QTranslator*> translators = m_translators.values();
    m_translators.clear();

    for(QTranslator* translator: translators)
    {
        qApp->removeTranslator(translator);
        delete translator;
    }

    return true;
}
