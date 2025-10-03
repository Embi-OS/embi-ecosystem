#include "qtranslatorloader.h"
#include "qutils_log.h"

#include <QDirIterator>
#include <QDir>
#include <QFile>
#include <QDebug>

Q_GLOBAL_STATIC_WITH_ARGS(QString, trFilePrefix, (":/i18n"))
Q_GLOBAL_STATIC_WITH_ARGS(QString, trFileSuffix, ("_%1.qm"))

QTranslatorLoader::QTranslatorLoader(QObject* parent) :
    QObject(parent)
{
    connect(this, &QTranslatorLoader::languageChanged, this, &QTranslatorLoader::onLanguageChanged);
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

    if(getLanguage().isEmpty())
        return false;

    const QStringList fileNames = getFileNames(getLanguage());
    for(const QString& fileName: fileNames)
    {
        if(!loadFile(fileName)) {
            QUTILSLOG_WARNING()<<"Failed to load"<<fileName<<"translation file, falling back to default";
        }
    }

    QUTILSLOG_INFO()<<"Load translation files:"<<fileNames;

    return true;
}

bool QTranslatorLoader::loadFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return false;

    QString name = fileName;
    name.remove(trFileSuffix->arg(getLanguage().toLower()));
    QTranslator* translator = m_translators.value(name);

    if(!translator)
    {
        translator = new QTranslator(this);
        m_translators[name] = translator;
        qApp->installTranslator(translator);
        QUTILSLOG_DEBUG()<<"Installed"<<translator<<name;
    }

    if(!QFile::exists(*trFilePrefix+"/"+fileName))
        return false;

    if(!translator->load(fileName, *trFilePrefix))
        return false;

    return true;
}

const QStringList QTranslatorLoader::getFileNames(const QString langage) const
{
    QDir dir(*trFilePrefix,"*"+trFileSuffix->arg(langage.toLower()));
    return dir.entryList();
}
