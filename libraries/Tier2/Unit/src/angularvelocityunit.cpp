#include "unitobject.h"
#include "unitcategoryobject.h"

class AngularVelocityUnitCategory : public UnitCategoryObject,
                                    public QSingleton<AngularVelocityUnitCategory>
{
friend QSingleton<AngularVelocityUnitCategory>;
    explicit AngularVelocityUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::AngularVelocity, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::AngularVelocity,
                                  UnitTypes::AngularVelocity_tr_min,
                                  UnitTypeObject::tr("Tours par minutes"),
                                  UnitTypeObject::tr("tr/m"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::AngularVelocity,
                                  UnitTypes::AngularVelocity_tr_s,
                                  UnitTypeObject::tr("Tours par secondes"),
                                  UnitTypeObject::tr("tr/s"),
                                  1.0/60.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::AngularVelocity,
                                  UnitTypes::AngularVelocity_deg_min,
                                  UnitTypeObject::tr("Degrés  par minutes"),
                                  UnitTypeObject::tr("°/m"),
                                  360.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::AngularVelocity,
                                  UnitTypes::AngularVelocity_deg_s,
                                  UnitTypeObject::tr("Degrés par secondes"),
                                  UnitTypeObject::tr("°/s"),
                                  1.0/360.0);
        items.append(item);

        setObjects(items);
    }
};

AngularVelocityUnit::AngularVelocityUnit(QObject *parent) :
    UnitObject(UnitCategories::AngularVelocity, parent)
{

}

UnitCategoryObject* AngularVelocityUnit::makeCategory()
{
    return AngularVelocityUnitCategory::Get();
}
