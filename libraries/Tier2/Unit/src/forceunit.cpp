#include "unitobject.h"
#include "unitcategoryobject.h"

class ForceUnitCategory : public UnitCategoryObject,
                          public QSingleton<ForceUnitCategory>
{
    friend QSingleton<ForceUnitCategory>;
    explicit ForceUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Force, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Force,
                                  UnitTypes::Force_N,
                                  UnitTypeObject::tr("Newtons"),
                                  UnitTypeObject::tr("N"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Force,
                                  UnitTypes::Force_mN,
                                  UnitTypeObject::tr("Millinewtons"),
                                  UnitTypeObject::tr("mN"),
                                  1000.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Force,
                                  UnitTypes::Force_uN,
                                  UnitTypeObject::tr("Micronewtons"),
                                  UnitTypeObject::tr("uN"),
                                  1000000.0);
        items.append(item);

        setObjects(items);
    }
};

ForceUnit::ForceUnit(QObject *parent) :
    UnitObject(UnitCategories::Force, parent)
{

}

UnitCategoryObject* ForceUnit::makeCategory()
{
    return ForceUnitCategory::Get();
}
