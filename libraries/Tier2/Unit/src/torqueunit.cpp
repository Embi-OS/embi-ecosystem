#include "unitobject.h"
#include "unitcategoryobject.h"

class TorqueUnitCategory : public UnitCategoryObject,
                           public QSingleton<TorqueUnitCategory>
{
    friend QSingleton<TorqueUnitCategory>;
    explicit TorqueUnitCategory(QObject *parent = nullptr) :
        UnitCategoryObject(UnitCategories::Torque, parent)
    {
        QList<UnitTypeObject*> items;
        UnitTypeObject* item=nullptr;

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_mNm,
                                  UnitTypeObject::tr("Millinewtons mètres"),
                                  UnitTypeObject::tr("mNm"),
                                  1.0);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_cNm,
                                  UnitTypeObject::tr("Centinewtons mètres"),
                                  UnitTypeObject::tr("cNm"),
                                  0.1);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_Ncm,
                                  UnitTypeObject::tr("Newtons centimètres"),
                                  UnitTypeObject::tr("Ncm"),
                                  0.1);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_Nm,
                                  UnitTypeObject::tr("Newtons mètres"),
                                  UnitTypeObject::tr("Nm"),
                                  0.001);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_gmm,
                                  UnitTypeObject::tr("Grammes millimètres"),
                                  UnitTypeObject::tr("gmm"),
                                  101.936799185);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_gcm,
                                  UnitTypeObject::tr("Grammes centimètres"),
                                  UnitTypeObject::tr("gcm"),
                                  10.1936799185);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_lbfin,
                                  UnitTypeObject::tr("Pounds force inch"),
                                  UnitTypeObject::tr("lbfin"),
                                  0.008849558);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_ozfin,
                                  UnitTypeObject::tr("Ounces force inch"),
                                  UnitTypeObject::tr("ozfin"),
                                  0.141643059);
        items.append(item);

        item = new UnitTypeObject(UnitCategories::Torque,
                                  UnitTypes::Torque_uNm,
                                  UnitTypeObject::tr("Micronewtons mètres"),
                                  UnitTypeObject::tr("uNm"),
                                  1000.0);
        items.append(item);

        setObjects(items);
    }
};

TorqueUnit::TorqueUnit(QObject *parent) :
    UnitObject(UnitCategories::Torque, parent)
{

}

UnitCategoryObject* TorqueUnit::makeCategory()
{
    return TorqueUnitCategory::Get();
}
