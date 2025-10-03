#include "analoginputhelper.h"

AnalogInputHelper::AnalogInputHelper(QObject * parent):
    QObject(parent),
    m_complete(false)
{

}

void AnalogInputHelper::classBegin()
{

}

void AnalogInputHelper::componentComplete()
{
    m_complete = true;
}

void AnalogInputHelper::clear()
{
    m_values.clear();
    m_averageStabilityTime.restart();
    m_medianStabilityTime.restart();
    resetLastSample();

    resetAverage();
    resetMedian();

    resetMaxValue();
    resetMaxValue();

    resetIsAverageStable();
    resetIsMedianStable();
}

void AnalogInputHelper::addSample(double value)
{
    if(!m_complete)
        return;

    // Add the new sample
    m_values.append(value);
    setLastSample(value);

    // Clean samples older than the specified window
    cleanValues();

    QList<double> sortedValues;
    sortedValues.reserve(m_values.size());

    resetMinValue();
    resetMaxValue();

    double sum = 0;
    for(const double& value: m_values)
    {
        if(value<m_minValue)
            setMinValue(value);
        if(value>m_maxValue)
            setMaxValue(value);
        sum += value;

        auto it = std::upper_bound(sortedValues.cbegin(), sortedValues.cend(), value);
        sortedValues.insert(it, value);
    }

    setAverage(sum / m_values.size());

    if (ABS(m_median - m_minValue) > ABS(m_stabilityThreshold) ||
        ABS(m_median - m_maxValue) > ABS(m_stabilityThreshold) ||
        m_values.size()<m_maxSize)
    {
        m_averageStabilityTime.restart();
    }
    setIsAverageStable(m_averageStabilityTime.nsecsElapsed()/1000000.0 > m_maxStabilityTime);

    double median = 0;
    int size = sortedValues.size();
    if (size % 2 == 0) {
        // If even number of elements, take the average of the two middle values
        median = (sortedValues[size / 2 - 1] + sortedValues[size / 2]) / 2.0;
    } else {
        // If odd number of elements, take the middle value
        median = sortedValues[size / 2];
    }

    setMedian(median);

    if (ABS(m_median - m_minValue) > ABS(m_stabilityThreshold) ||
        ABS(m_median - m_maxValue) > ABS(m_stabilityThreshold) ||
        m_values.size()<m_maxSize)
    {
        m_averageStabilityTime.restart();
    }
    setIsMedianStable(m_averageStabilityTime.nsecsElapsed()/1000000.0 > m_maxStabilityTime);
}

void AnalogInputHelper::cleanValues()
{
    if(m_values.size()<=m_maxSize)
        return;

    m_values.remove(0, m_values.size()-m_maxSize);
}
