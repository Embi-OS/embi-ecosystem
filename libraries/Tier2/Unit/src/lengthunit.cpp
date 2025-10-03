#include "unitobject.h"
#include "unitcategoryobject.h"

class LengthUnitCategory : public UnitCategoryObject,
                           public QSingleton<LengthUnitCategory>
{
    friend QSingleton<LengthUnitCategory>;
    explicit LengthUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Length, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Length,
                                  UnitTypes::Length_mm,
                                  UnitTypeObject::tr("Millimètres"),
                                  UnitTypeObject::tr("mm"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Length,
                                  UnitTypes::Length_um,
                                  UnitTypeObject::tr("Micromètres"),
                                  UnitTypeObject::tr("um"),
                                  1000.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Length,
                                  UnitTypes::Length_m,
                                  UnitTypeObject::tr("Mètres"),
                                  UnitTypeObject::tr("m"),
                                  0.001);
        items.append(item);

        setObjects(items);
    }
};

LengthUnit::LengthUnit(QObject *parent) :
    UnitObject(UnitCategories::Length, parent)
{

}

UnitCategoryObject* LengthUnit::makeCategory()
{
    return LengthUnitCategory::Get();
}
