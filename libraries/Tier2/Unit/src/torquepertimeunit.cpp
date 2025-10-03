#include "unitobject.h"
#include "unitcategoryobject.h"

class TorquePerTimeUnitCategory : public UnitCategoryObject,
                                  public QSingleton<TorquePerTimeUnitCategory>
{
    friend QSingleton<TorquePerTimeUnitCategory>;
    explicit TorquePerTimeUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::TorquePerTime, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::TorquePerTime,
                                  UnitTypes::TorquePerTime_mNm_sec,
                                  UnitTypeObject::tr("Millinewtons mètres par secondes"),
                                  UnitTypeObject::tr("mNm/s"),
                                  1.0);
        items.append(item);

        setObjects(items);
    }
};

TorquePerTimeUnit::TorquePerTimeUnit(QObject *parent) :
    UnitObject(UnitCategories::TorquePerTime, parent)
{

}

UnitCategoryObject* TorquePerTimeUnit::makeCategory()
{
    return TorquePerTimeUnitCategory::Get();
}
