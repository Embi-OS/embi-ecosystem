#include "localemodel.h"

bool localeLessThan(const LocaleObject* left, const LocaleObject* right) {
    return left->getName() < right->getName();
}

LocaleObject::LocaleObject(const QLocale& locale, QObject *parent) :
    QObject(parent),
    m_locale(locale),
    m_name(locale.name()),
    m_territory(locale.nativeTerritoryName()),
    m_language(locale.nativeLanguageName()),
    m_lang(locale.name().split("_").at(0)),
    m_display(QString("%1 (%2)").arg(m_language, m_name))
{
    if(m_territory.isEmpty()) {
        m_territory = QLocale::territoryToString(locale.territory());
    }

    if(m_language.isEmpty()) {
        m_language = QLocale::languageToString(locale.language());
    }
};

LocaleModel::LocaleModel(QObject* parent) :
    QObjectListModel(parent, &LocaleObject::staticMetaObject)
{
    refresh();
}

void LocaleModel::refresh()
{
    const QList<LocaleObject*> locales = LocaleModel::generateModel();
    setObjects(locales);
    emit ready();
}

QModelIndex LocaleModel::indexForTerritory(const QString &territory) const
{
    for(LocaleObject* locale: modelIterator<LocaleObject>())
    {
        if (locale->getTerritory() == territory ||
            locale->getLanguage() == territory)
        {
            return index(indexOf(locale));
        }
    }
    return QModelIndex();
}

QList<LocaleObject*> LocaleModel::generateModel()
{
    const QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::LatinScript, QLocale::AnyTerritory);

    QList<LocaleObject*> toAppend;
    toAppend.reserve(locales.size());
    for(const QLocale &locale : locales)
    {
        if(QLocale::system()==locale)
            continue;
        if(locale.nativeLanguageName()!="") {
            LocaleObject *object = new LocaleObject(locale);
            toAppend.append(object);
        }
    }

    std::stable_sort(toAppend.begin(),toAppend.end(),localeLessThan);

    return toAppend;
}
