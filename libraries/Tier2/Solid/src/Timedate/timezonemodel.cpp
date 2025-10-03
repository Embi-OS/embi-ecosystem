#include "timezonemodel.h"

bool timezoneLessThan(const TimezoneObject* left, const TimezoneObject* right) {
    return left->getIdentifier() < right->getIdentifier();
}

TimezoneObject::TimezoneObject(const QByteArray& id, QObject *parent) :
    QObject(parent),
    m_identifier(id)
{

}

const QString& TimezoneObject::getIdentifier() const
{
    return m_identifier;
}

const QTimeZone& TimezoneObject::getTimezone() const
{
    if(!m_timezone.isValid())
        m_timezone = QTimeZone(m_identifier.toUtf8());
    return m_timezone;
}

const QString& TimezoneObject::getName() const
{
    if(m_name.isEmpty())
        m_name = getTimezone().displayName(QTimeZone::StandardTime);
    return m_name;
}

const QString& TimezoneObject::getTerritory() const
{
    if(m_territory.isEmpty())
        m_territory = QLocale::territoryToString(getTimezone().territory());
    return m_territory;
}

TimezoneModel::TimezoneModel(QObject* parent) :
    QObjectListModel(parent, &TimezoneObject::staticMetaObject)
{
    refresh();
}

void TimezoneModel::refresh()
{
    const QList<TimezoneObject*> timezones = TimezoneModel::generateModel();
    setObjects(timezones);
    emit ready();
}

QModelIndex TimezoneModel::indexForTimezone(const QString &timezone) const
{
    for(TimezoneObject* locale: modelIterator<TimezoneObject>())
    {
        if (locale->getIdentifier() == timezone)
        {
            return index(indexOf(locale));
        }
    }
    return QModelIndex();
}

QList<TimezoneObject*> TimezoneModel::generateModel()
{
    const QByteArrayList ids = QTimeZone::availableTimeZoneIds();

    QList<TimezoneObject*> toAppend;
    toAppend.reserve(ids.size());
    for(const QByteArray& id: ids)
    {
        TimezoneObject *object = new TimezoneObject(id);
        toAppend.append(object);
    }
    std::stable_sort(toAppend.begin(),toAppend.end(),timezoneLessThan);

    return toAppend;
}

