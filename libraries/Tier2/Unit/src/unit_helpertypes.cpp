#include "unit_helpertypes.h"

UnitValueType::UnitValueType():
    UnitValueType(UnitCategories::NoCategory, 0, UnitTypes::NoUnit)
{

}

UnitValueType::UnitValueType(UnitCategory category):
    UnitValueType(category, 0, (UnitTypes::Enum)category)
{

}

UnitValueType::UnitValueType(UnitCategory category, double value, UnitType type):
    m_category(category),
    m_value(value),
    m_type(type)
{

}

UnitValueType::UnitValueType(const UnitValueType &other):
    UnitValueType(other.m_category, other.m_value, other.m_type)
{

}

QString UnitValueType::toString() const
{
    return QString("Category: %1 - Value: %2 [%3]").arg(UnitCategories::asString(m_category), QString::number(m_value, 'g', QLocale::FloatingPointShortest), UnitTypes::asString(m_type));
}

UnitCategory UnitValueType::getCategory() const
{
    return m_category;
}
bool UnitValueType::setCategory(UnitCategory category)
{
    if(m_category==category)
        return false;
    m_category = category;
    return true;
}

double UnitValueType::getValue() const
{
    return m_value;
}
bool UnitValueType::setValue(double value)
{
    if(m_value==value)
        return false;
    m_value = value;
    return true;
}

UnitType UnitValueType::getType() const
{
    return m_type;
}
bool UnitValueType::setType(UnitType type)
{
    if(m_type==type)
        return false;
    m_type = type;
    return true;
}

UnitValueType& UnitValueType::operator=(const UnitValueType &other)
{
    m_category = other.m_category;
    m_value = other.m_value;
    m_type = other.m_type;
    return *this;
}

QDebug operator<<(QDebug dbg, const UnitValueType &unitValueType)
{
    dbg.nospace()<<"UnitValueType("<<unitValueType.toString()<<")";
    return dbg.space();
}
