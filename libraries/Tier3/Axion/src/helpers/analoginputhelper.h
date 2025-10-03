#ifndef ANALOGINPUTHELPER_H
#define ANALOGINPUTHELPER_H

#include <QDefs>

class AnalogInputHelper : public QObject,
                          public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES (QQmlParserStatus)

    Q_READONLY_FUZ_PROPERTY(double, lastSample, LastSample, 0)

    Q_READONLY_FUZ_PROPERTY(double, average, Average, 0)
    Q_READONLY_VAR_PROPERTY(bool, isAverageStable, IsAverageStable, false)

    Q_READONLY_FUZ_PROPERTY(double, median, Median, 0)
    Q_READONLY_VAR_PROPERTY(bool, isMedianStable, IsMedianStable, false)

    Q_READONLY_FUZ_PROPERTY(double, minValue, MinValue, MAX_DOUBLE_VAL)
    Q_READONLY_FUZ_PROPERTY(double, maxValue, MaxValue, -MAX_DOUBLE_VAL)

    Q_WRITABLE_VAR_PROPERTY(int, maxSize, MaxSize, 0)
    Q_WRITABLE_VAR_PROPERTY(int, maxStabilityTime, MaxStabilityTime, 0)
    Q_WRITABLE_VAR_PROPERTY(double, stabilityThreshold, StabilityThreshold, 0)

public:
    explicit AnalogInputHelper(QObject* parent=nullptr);

    void classBegin() override;
    void componentComplete() override;

public slots:
    void clear();
    void addSample(double value);

signals:
    void throttled();

private:
    void cleanValues();

    QList<double> m_values;
    QElapsedTimer m_averageStabilityTime;
    QElapsedTimer m_medianStabilityTime;

    bool m_complete;
};

#endif // ANALOGINPUTHELPER_H
