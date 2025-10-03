#include "unitobject.h"
#include "unitcategoryobject.h"

class TimeUnitCategory : public UnitCategoryObject,
                         public QSingleton<TimeUnitCategory>
{
    friend QSingleton<TimeUnitCategory>;
    explicit TimeUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Time, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Time,
                                  UnitTypes::Time_sec,
                                  UnitTypeObject::tr("Secondes"),
                                  UnitTypeObject::tr("s"),
                                  0.001);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Time,
                                  UnitTypes::Time_min,
                                  UnitTypeObject::tr("Minutes"),
                                  UnitTypeObject::tr("m"),
                                  0.001/60.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Time,
                                  UnitTypes::Time_hour,
                                  UnitTypeObject::tr("Heures"),
                                  UnitTypeObject::tr("h"),
                                  0.001/3600.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Time,
                                  UnitTypes::Time_msec,
                                  UnitTypeObject::tr("Millisecondes"),
                                  UnitTypeObject::tr("ms"),
                                  1.0);
        items.append(item);

        setObjects(items);
    }
};

TimeUnit::TimeUnit(QObject *parent) :
    UnitObject(UnitCategories::Time, parent)
{

}

UnitCategoryObject* TimeUnit::makeCategory()
{
    return TimeUnitCategory::Get();
}
