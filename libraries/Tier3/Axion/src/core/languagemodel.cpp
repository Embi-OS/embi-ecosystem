#include "languagemodel.h"

#include "qtranslatorloader.h"

#include <algorithm>
#include <iterator>
#include <QLocale>
#include <utility>

bool languageLessThan(const LanguageObject* left, const LanguageObject* right) {
    return left->name() < right->name();
}

QString languageDisplayName(const QString& name)
{
    if(name == QStringLiteral("system"))
        return LanguageObject::tr("Système");

    if(name == QStringLiteral("c"))
        return LanguageObject::tr("Source");

    const QLocale locale(name);
    QString display = locale.nativeLanguageName();
    if(display.isEmpty() || locale.language() == QLocale::C)
        display = QLocale::languageToString(locale.language());

    if(display.isEmpty())
        return name;

    return display;
}

LanguageObject::LanguageObject(const QString& name, QObject *parent) :
    QObject(parent),
    m_name(name)
{

}

QString LanguageObject::language() const
{
    return languageDisplayName(m_name);
}

QString LanguageObject::display() const
{
    return QString("%1 (%2)").arg(language(), m_name);
}

LanguageModel::LanguageModel(QObject* parent) :
    QObjectListModel(parent, &LanguageObject::staticMetaObject)
{
    refresh();
}

void LanguageModel::refresh()
{
    const QList<LanguageObject*> languages = LanguageModel::generateModel();
    setObjects(languages);
    emit ready();
}

QList<LanguageObject*> LanguageModel::generateModel()
{
    const QStringList languages = QTranslatorLoader::availableLanguages();

    QList<LanguageObject*> objects;
    objects.reserve(languages.size() + 1);
    objects.append(new LanguageObject(QStringLiteral("system")));

    for(const QString& language: std::as_const(languages))
        objects.append(new LanguageObject(language));

    std::stable_sort(std::next(objects.begin()), objects.end(), languageLessThan);

    return objects;
}
