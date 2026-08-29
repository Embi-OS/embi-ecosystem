#include "units.h"
#include "unit_log.h"

Units::Units(QObject *parent) :
    QObject(parent)
{

}

UnitCategoryObject* Units::percentUnitModel()
{
    return loadCategory(m_percentUnitModel, "PercentUnitCategory");
}
UnitCategoryObject* Units::torqueUnitModel()
{
    return loadCategory(m_torqueUnitModel, "TorqueUnitCategory");
}
UnitCategoryObject* Units::torquePerTimeUnitModel()
{
    return loadCategory(m_torquePerTimeUnitModel, "TorquePerTimeUnitCategory");
}
UnitCategoryObject* Units::angularVelocityUnitModel()
{
    return loadCategory(m_angularVelocityUnitModel, "AngularVelocityUnitCategory");
}
UnitCategoryObject* Units::angleUnitModel()
{
    return loadCategory(m_angleUnitModel, "AngleUnitCategory");
}
UnitCategoryObject* Units::timeUnitModel()
{
    return loadCategory(m_timeUnitModel, "TimeUnitCategory");
}
UnitCategoryObject* Units::massUnitModel()
{
    return loadCategory(m_massUnitModel, "MassUnitCategory");
}
UnitCategoryObject* Units::forceUnitModel()
{
    return loadCategory(m_forceUnitModel, "ForceUnitCategory");
}
UnitCategoryObject* Units::lengthUnitModel()
{
    return loadCategory(m_lengthUnitModel, "LengthUnitCategory");
}

UnitCategoryObject* Units::loadCategory(UnitCategoryObject*& storage, const QString& qmlType)
{
    if(!storage)
        storage = QQmlLoader::load<UnitCategoryObject>("Eco.Tier2.Unit", qmlType, this);

    return storage;
}

UnitCategoryObject* Units::make(UnitCategories::Enum category)
{
    switch(category)
    {
    case UnitCategories::Percent:
        return Units::Get()->percentUnitModel();
    case UnitCategories::Torque:
        return Units::Get()->torqueUnitModel();
    case UnitCategories::TorquePerTime:
        return Units::Get()->torquePerTimeUnitModel();
    case UnitCategories::AngularVelocity:
        return Units::Get()->angularVelocityUnitModel();
    case UnitCategories::Angle:
        return Units::Get()->angleUnitModel();
    case UnitCategories::Time:
        return Units::Get()->timeUnitModel();
    case UnitCategories::Mass:
        return Units::Get()->massUnitModel();
    case UnitCategories::Force:
        return Units::Get()->forceUnitModel();
    case UnitCategories::Length:
        return Units::Get()->lengthUnitModel();
    default:
        UNITLOG_CRITICAL()<<"No CategoryObject provided for:"<<category;
        return nullptr;
    }

    return nullptr;
}
