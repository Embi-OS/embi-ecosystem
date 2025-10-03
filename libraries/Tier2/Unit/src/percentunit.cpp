#include "unitobject.h"
#include "unitcategoryobject.h"

class PercentUnitCategory : public UnitCategoryObject,
                          public QSingleton<PercentUnitCategory>
{
    friend QSingleton<PercentUnitCategory>;
    explicit PercentUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Percent, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Percent,
                                  UnitTypes::Percent,
                                  UnitTypeObject::tr("Pourcent"),
                                  UnitTypeObject::tr("%"),
                                  1.0);
        items.append(item);

        setObjects(items);
    }
};

PercentUnit::PercentUnit(QObject *parent) :
    UnitObject(UnitCategories::Percent, parent)
{

}

UnitCategoryObject* PercentUnit::makeCategory()
{
    return PercentUnitCategory::Get();
}
