#include "unitobject.h"
#include "units.h"

UnitObject::UnitObject(UnitCategories::Enum category, QObject *parent) :
    QObject(parent),
    m_category(category),
    m_categoryObject(Units::make(category)),
    m_defaultType((UnitTypes::Enum)category),
    m_type((UnitTypes::Enum)category),
    m_throttler(new QSignalTrailingThrottler(this))
{
    connect(this, &UnitObject::rawValueChanged, this, &UnitObject::updateDisplay);
    connect(this, &UnitObject::decimalsChanged, this, &UnitObject::updateDisplay);
    connect(this, &UnitObject::typeChanged, this, &UnitObject::updateDisplay);

    updateDisplay();

    m_throttler->setTimeout(32);
    connect(this, &UnitObject::displayUpdated, m_throttler, &QGenericSignalThrottler::throttle);
    connect(m_throttler, &QGenericSignalThrottler::triggered, this, &UnitObject::throttled);
}

double UnitObject::getValue(UnitTypes::Enum type) const
{
    if(type==UnitTypes::NoUnit || type==getCategoryObject()->getDefaultType())
        return getRawValue();

    return getCategoryObject()->convertTo(getRawValue(), type);
}

bool UnitObject::setValue(double value, UnitTypes::Enum type)
{
    if(type==UnitTypes::NoUnit || type==getCategoryObject()->getDefaultType())
        return setRawValue(value);

    return setRawValue(getCategoryObject()->convertFrom(value, type));
}

void UnitObject::updateDisplay()
{
    setDisplayType(getCategoryObject()->abbreviation(m_type));
    setDisplayValue(qFuzzyRound(getCategoryObject()->convertTo(m_rawValue,getType()), m_decimals));
    setFormattedValue(QString("%1").arg(QString::number(m_displayValue, 'g', QLocale::FloatingPointShortest)));
    setDisplay(QString("%1 [%2]").arg(m_formattedValue, m_displayType));

    emit this->displayUpdated();
}
