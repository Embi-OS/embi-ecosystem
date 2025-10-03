#include "unitobject.h"
#include "unitcategoryobject.h"

class MassUnitCategory : public UnitCategoryObject,
                          public QSingleton<MassUnitCategory>
{
    friend QSingleton<MassUnitCategory>;
    explicit MassUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Mass, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Mass,
                                  UnitTypes::Mass_gr,
                                  UnitTypeObject::tr("Grams"),
                                  UnitTypeObject::tr("gr"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Mass,
                                  UnitTypes::Mass_kg,
                                  UnitTypeObject::tr("Kilograms"),
                                  UnitTypeObject::tr("kg"),
                                  0.001);
        items.append(item);

        setObjects(items);
    }
};

MassUnit::MassUnit(QObject *parent) :
    UnitObject(UnitCategories::Mass, parent)
{

}

UnitCategoryObject* MassUnit::makeCategory()
{
    return MassUnitCategory::Get();
}
