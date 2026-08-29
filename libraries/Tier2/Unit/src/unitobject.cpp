#include "unitobject.h"
#include "units.h"

UnitObject::UnitObject(UnitCategories::Enum category, QObject *parent) :
    QObject(parent),
    m_categoryObject(Units::make(category)),
    m_category(category),
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
    if(type==UnitTypes::NoUnit || type==m_categoryObject->getDefaultType())
        return getRawValue();

    return m_categoryObject->convertTo(getRawValue(), type);
}

bool UnitObject::setValue(double value, UnitTypes::Enum type)
{
    if(type==UnitTypes::NoUnit || type==m_categoryObject->getDefaultType())
        return setRawValue(value);

    return setRawValue(m_categoryObject->convertFrom(value, type));
}

void UnitObject::updateDisplay()
{
    setDisplayType(m_categoryObject->abbreviation(m_type));
    setDisplayValue(qFuzzyRound(m_categoryObject->convertTo(m_rawValue,getType()), m_decimals));
    setFormattedValue(QString("%1").arg(QString::number(m_displayValue, 'g', QLocale::FloatingPointShortest)));
    setDisplay(QString("%1 [%2]").arg(m_formattedValue, m_displayType));

    emit this->displayUpdated();
}
