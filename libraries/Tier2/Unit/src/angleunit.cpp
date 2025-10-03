#include "unitobject.h"
#include "unitcategoryobject.h"

class AngleUnitCategory : public UnitCategoryObject,
                          public QSingleton<AngleUnitCategory>
{
    friend QSingleton<AngleUnitCategory>;
    explicit AngleUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Angle, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Angle,
                                  UnitTypes::Angle_deg,
                                  UnitTypeObject::tr("Degrés"),
                                  UnitTypeObject::tr("°"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Angle,
                                  UnitTypes::Angle_tr,
                                  UnitTypeObject::tr("Tours"),
                                  UnitTypeObject::tr("tr"),
                                  1.0/360.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Angle,
                                  UnitTypes::Angle_rad,
                                  UnitTypeObject::tr("Radians"),
                                  UnitTypeObject::tr("rad"),
                                  M_PI/180.0);
        items.append(item);

        setObjects(items);
    }
};

AngleUnit::AngleUnit(QObject *parent) :
    UnitObject(UnitCategories::Angle, parent)
{

}

UnitCategoryObject* AngleUnit::makeCategory()
{
    return AngleUnitCategory::Get();
}
