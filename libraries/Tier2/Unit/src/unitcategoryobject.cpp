#include "unitcategoryobject.h"
#include "unit_log.h"

UnitCategoryObject::UnitCategoryObject(UnitCategories::Enum category, QObject *parent) :
    QObjectListModel(parent, &UnitTypeObject::staticMetaObject),
    m_category(category),
    m_defaultType((UnitTypes::Enum)category)
{

}

bool UnitCategoryObject::checkCategoryCompatibility(const UnitCategories::Enum& category) const
{
    if(category!=getCategory())
    {
        UNITLOG_WARNING()<<"Try to use wrong unit category("<<category<<")=> fallback to default Category";
        return false;
    }
    return true;
}

bool UnitCategoryObject::checkTypeCompatibility(UnitTypes::Enum type) const
{
    if(type>=0 && type<1000)
        return true;

    if(type-getDefaultType()<0 || type-getDefaultType()>=1000)
    {
        UNITLOG_WARNING()<<"Try to use unit type from wrong category("<<type<<")=> fallback to default Unit";
        return false;
    }
    return true;
}

UnitTypes::Enum UnitCategoryObject::type(int index) const
{
    if(index < 0 || index >= size())
        return (UnitType)m_category; //UnitTypes::InvalidUnit;

    if(UnitTypeObject* object = at<UnitTypeObject>(index))
        return object->getType();

    return UnitTypes::InvalidUnit;
}

QString UnitCategoryObject::name(UnitTypes::Enum type) const
{
    if(!checkTypeCompatibility(type))
    {
        return "N/A";
    }

    if(UnitTypeObject* object = unitObject(type))
        return object->getName();

    return "N/A";
}

QString UnitCategoryObject::abbreviation(UnitTypes::Enum type) const
{
    if(!checkTypeCompatibility(type))
    {
        return "N/A";
    }

    if(UnitTypeObject* object = unitObject(type))
        return object->getAbbreviation();

    return "N/A";
}

double UnitCategoryObject::ratio(UnitTypes::Enum type) const
{
    if(!checkTypeCompatibility(type))
    {
        return DOUBLE_NAN;
    }

    if(UnitTypeObject* object = unitObject(type))
    {
        double ratio = object->getRatio();
        if (qFuzzyCompare(ratio, 0))
            ratio=1.0;
        return ratio;
    }

    return DOUBLE_NAN;
}

QString UnitCategoryObject::rounded(double rawValue, int decimals) const
{
    const QString roundedValue = QString::number(rawValue, 'g', decimals);

    return roundedValue;
}

QString UnitCategoryObject::formatted(double rawValue, UnitTypes::Enum type, int decimals) const
{
    double displayValue = convertTo(rawValue, type);
    displayValue = decimals<0 ? displayValue : qFuzzyRound(displayValue, decimals);
    const QString formattedValue = QString::number(displayValue, 'g', decimals);

    return formattedValue;
}

QString UnitCategoryObject::display(double rawValue, UnitTypes::Enum type, int decimals) const
{
    const QString displayType = abbreviation(type);
    double displayValue = convertTo(rawValue, type);
    displayValue = decimals<0 ? displayValue : qFuzzyRound(displayValue, decimals);
    const QString formattedValue = QString::number(displayValue, 'g', decimals);

    return QString("%1 [%2]").arg(formattedValue, displayType);
}

double UnitCategoryObject::convert(double value, UnitTypes::Enum fromType, UnitTypes::Enum toType) const
{
    return convertTo(convertFrom(value, fromType), toType);
}

double UnitCategoryObject::convertTo(double value, UnitTypes::Enum type) const
{
    return value*ratio(type);
}

double UnitCategoryObject::convertFrom(double value, UnitTypes::Enum type) const
{
    return value/ratio(type);
}

void UnitCategoryObject::onObjectAboutToBeInserted(QObject* object, int row)
{
    Q_UNUSED(row)
    if(UnitTypeObject* castObject = qobject_cast<UnitTypeObject*>(object))
        checkTypeCompatibility(castObject->getType());
}

UnitTypeObject* UnitCategoryObject::unitObject(UnitTypes::Enum type) const
{
    for(UnitTypeObject* object: modelIterator<UnitTypeObject>())
    {
        if(object->getType()==type)
            return object;
    }

    return nullptr;
}
