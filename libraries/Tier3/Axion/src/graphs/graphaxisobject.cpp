#include "graphaxisobject.h"

GraphAxisObject::GraphAxisObject(UnitCategory category, QObject* parent) :
    QObject(parent),
    m_upperBoundRaw(1),
    m_throttler(new QSignalTrailingThrottler(this)),
    m_boundsThrottler(new QSignalTrailingThrottler(this))
{
    m_category = category;
    m_unit = (UnitType)category;

    connect(this, &GraphAxisObject::globalMaxRawChanged, this, &GraphAxisObject::updateBounds);
    connect(this, &GraphAxisObject::globalMinRawChanged, this, &GraphAxisObject::updateBounds);

    m_boundsThrottler->setTimeout(32);
    connect(this, &GraphAxisObject::lowerScopedChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::upperScopedChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::globalMaxRawChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::globalMinRawChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::lowerBoundRawChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::upperBoundRawChanged, m_boundsThrottler, &QGenericSignalThrottler::throttle);
    connect(m_boundsThrottler, &QGenericSignalThrottler::triggered, this, &GraphAxisObject::boundsThrottled);

    m_throttler->setTimeout(32);
    connect(this, &GraphAxisObject::globalMaxRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::globalMinRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphAxisObject::averageRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(m_throttler, &QGenericSignalThrottler::triggered, this, &GraphAxisObject::throttled);
}

void GraphAxisObject::reset()
{
    setIsAbleToMinMax(true);
    setIsAverageable(true);
    setIsScalable(true);

    resetBounds();
    resetMinMax();

    setAverageRaw(0);
    m_averagePointCount = 0;
}

void GraphAxisObject::resetBounds()
{
    setLowerMarginPct(0);
    setUpperMarginPct(0);
    setLowerBoundRaw(0);
    setUpperBoundRaw(0);
}

void GraphAxisObject::resetMinMax()
{
    setGlobalMaxRaw(MIN_FLOAT_VAL);
    setGlobalMinRaw(MAX_FLOAT_VAL);
}

float GraphAxisObject::convertRawValue(float valueRaw) const
{
    return getConverterFunction()(valueRaw);
}

GraphAxisConvertFunction GraphAxisObject::getConverterFunction() const
{
    float ratio = Units::make(getCategory())->ratio(getUnit());

    GraphAxisConvertFunction f = [ratio](float valueRaw) {
        return valueRaw*ratio;
    };

    return f;
}

QString GraphAxisObject::getUnitLabel() const
{
    return Units::make(getCategory())->abbreviation(getUnit());
}

float GraphAxisObject::getLowerBound() const
{
    float lowerBoundRaw = m_lowerScoped ? getGlobalMinRaw() : getLowerBoundRaw();
    float upperBoundRaw = m_lowerScoped ? getGlobalMaxRaw() : getUpperBoundRaw();
    float aDelta = ABS(upperBoundRaw-lowerBoundRaw);
    float lowerBound = lowerBoundRaw-(aDelta/100)*getLowerMarginPct();

    return convertRawValue(lowerBound);
}

float GraphAxisObject::getUpperBound() const
{
    float lowerBoundRaw = m_upperScoped ? getGlobalMinRaw() : getLowerBoundRaw();
    float upperBoundRaw = m_upperScoped ? getGlobalMaxRaw() : getUpperBoundRaw();
    float aDelta = ABS(upperBoundRaw-lowerBoundRaw);
    float upperBound = upperBoundRaw+(aDelta/100)*getUpperMarginPct();

    return convertRawValue(upperBound);
}

float GraphAxisObject::getGlobalMax() const
{
    return convertRawValue(getGlobalMaxRaw());
}

float GraphAxisObject::getGlobalMin() const
{
    return convertRawValue(getGlobalMinRaw());
}

float GraphAxisObject::getAverage() const
{
    return convertRawValue(getAverageRaw());
}

void GraphAxisObject::addValue(float value)
{
    updateMinMax(value);
    updateBounds(value);
    updateAverage(value);
}

void GraphAxisObject::updateMinMax(float value)
{
    if(!getIsAbleToMinMax())
        return;

    if(value>getGlobalMaxRaw())
    {
        setGlobalMaxRaw(value);
    }

    if(value<getGlobalMinRaw())
    {
        setGlobalMinRaw(value);
    }
}

void GraphAxisObject::updateUpperBound(float value)
{
    if(!getIsScalable())
        return;

    if(value>getUpperBoundRaw())
    {
        setUpperBoundRaw(value);
    }
}

void GraphAxisObject::updateLowerBound(float value)
{
    if(!getIsScalable())
        return;

    if(value<getLowerBoundRaw())
    {
        setLowerBoundRaw(value);
    }
}

void GraphAxisObject::updateBounds(float value)
{
    if(!getIsScalable())
        return;

    updateUpperBound(value);
    updateLowerBound(value);
}

void GraphAxisObject::updateAverage(float value)
{
    if(!getIsAverageable())
        return;

    double sum = getAverageRaw()*float(m_averagePointCount);
    sum += value;

    m_averagePointCount++;
    setAverageRaw(sum/float(m_averagePointCount));
}
