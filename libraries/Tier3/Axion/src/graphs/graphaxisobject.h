#ifndef GRAPHAXISOBJECT_H
#define GRAPHAXISOBJECT_H

#include <QDefs>
#include <QUtils>
#include <Unit>

typedef std::function<float(float)> GraphAxisConvertFunction;

class GraphAxisObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_VAR_PROPERTY(UnitCategories::Enum, category, Category, UnitCategories::NoCategory)
    Q_READONLY_VAR_PROPERTY(UnitTypes::Enum, unit, Unit, UnitTypes::NoUnit)
    Q_READONLY_FUZ_PROPERTY(float, lowerBoundRaw, LowerBoundRaw, 0)
    Q_READONLY_FUZ_PROPERTY(float, lowerMarginPct, LowerMarginPct, 0)
    Q_READONLY_FUZ_PROPERTY(float, upperBoundRaw, UpperBoundRaw, 0)
    Q_READONLY_FUZ_PROPERTY(float, upperMarginPct, UpperMarginPct, 0)
    Q_READONLY_VAR_PROPERTY(float, globalMaxRaw, GlobalMaxRaw, {})
    Q_READONLY_VAR_PROPERTY(float, globalMinRaw, GlobalMinRaw, {})
    Q_READONLY_FUZ_PROPERTY(float, averageRaw, AverageRaw, 0)

    Q_READONLY_FUZ_PROPERTY(float, aliasingValue, AliasingValue, 0)

    Q_WRITABLE_VAR_PROPERTY(bool, lowerScoped, LowerScoped, false)
    Q_WRITABLE_VAR_PROPERTY(bool, upperScoped, UpperScoped, false)

    Q_READONLY_VAR_PROPERTY(bool, isScalable, IsScalable, true)
    Q_READONLY_VAR_PROPERTY(bool, isAverageable, IsAverageable, true)
    Q_READONLY_VAR_PROPERTY(bool, isAbleToMinMax, IsAbleToMinMax, true)

    Q_PROPERTY(QString unitLabel READ getUnitLabel NOTIFY unitChanged FINAL)
    Q_PROPERTY(float lowerBound READ getLowerBound NOTIFY boundsThrottled FINAL)
    Q_PROPERTY(float upperBound READ getUpperBound NOTIFY boundsThrottled FINAL)
    Q_PROPERTY(float globalMax READ getGlobalMax NOTIFY throttled FINAL)
    Q_PROPERTY(float globalMin READ getGlobalMin NOTIFY throttled FINAL)
    Q_PROPERTY(float average READ getAverage NOTIFY throttled FINAL)

    Q_CONSTANT_PTR_PROPERTY(QGenericSignalThrottler, throttler, Throttler, nullptr)
    Q_CONSTANT_PTR_PROPERTY(QGenericSignalThrottler, boundsThrottler, BoundsThrottler, nullptr)

public:
    explicit GraphAxisObject(UnitCategory category, QObject* parent = nullptr);

    void reset();
    void resetBounds();
    void resetMinMax();

    Q_INVOKABLE float convertRawValue(float valueRaw) const;
    GraphAxisConvertFunction getConverterFunction() const;

    QString getUnitLabel() const;
    float getLowerBound() const;
    float getUpperBound() const;
    float getGlobalMax() const;
    float getGlobalMin() const;
    float getAverage() const;

public slots:
    void addValue(float value);

signals:
    void throttled();
    void boundsThrottled();

private slots:
    void updateMinMax(float value);
    void updateUpperBound(float value);
    void updateLowerBound(float value);
    void updateBounds(float value);
    void updateAverage(float value);

private:
    long m_averagePointCount;
};

#endif // GRAPHAXISOBJECT_H
