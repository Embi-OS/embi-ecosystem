#include "units.h"
#include "unit_log.h"
#include "unitobject.h"

Units::Units(QObject *parent) :
    QObject(parent),
    m_percentUnitModel(make(UnitCategories::Percent)),
    m_torqueUnitModel(make(UnitCategories::Torque)),
    m_torquePerTimeUnitModel(make(UnitCategories::TorquePerTime)),
    m_angularVelocityUnitModel(make(UnitCategories::AngularVelocity)),
    m_angleUnitModel(make(UnitCategories::Angle)),
    m_timeUnitModel(make(UnitCategories::Time)),
    m_massUnitModel(make(UnitCategories::Mass)),
    m_forceUnitModel(make(UnitCategories::Force)),
    m_lengthUnitModel(make(UnitCategories::Length))
{

}

UnitCategoryObject* Units::make(UnitCategories::Enum category)
{
    switch(category)
    {
    case UnitCategories::Percent:
        return PercentUnit::makeCategory();
    case UnitCategories::Torque:
        return TorqueUnit::makeCategory();
    case UnitCategories::TorquePerTime:
        return TorquePerTimeUnit::makeCategory();
    case UnitCategories::AngularVelocity:
        return AngularVelocityUnit::makeCategory();
    case UnitCategories::Angle:
        return AngleUnit::makeCategory();
    case UnitCategories::Time:
        return TimeUnit::makeCategory();
    case UnitCategories::Mass:
        return MassUnit::makeCategory();
    case UnitCategories::Force:
        return ForceUnit::makeCategory();
    case UnitCategories::Length:
        return LengthUnit::makeCategory();
    default:
        UNITLOG_CRITICAL()<<"No CategoryObject provided for:"<<category;
        return nullptr;
    }

    return nullptr;
}
