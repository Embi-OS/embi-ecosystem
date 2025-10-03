#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QDefs>
#include "qsingleton.h"

class MathUtils : public QObject,
                  public QQmlSingleton<MathUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<MathUtils>;
    explicit MathUtils(QObject *parent = nullptr);

public:
    Q_INVOKABLE double abs(double val) const;
    Q_INVOKABLE double round(double val) const;
    Q_INVOKABLE double sign(double val) const;
    Q_INVOKABLE double bound(double min, double val, double max) const;

    Q_INVOKABLE double fuzzyModulo(double p1, double p2, double precision=12) const;
    Q_INVOKABLE bool fuzzyCompare(double p1, double p2, double precision=12) const;
    Q_INVOKABLE bool fuzzyIsNull(double d, double precision=12) const;

    Q_INVOKABLE QPointF linearRegression(const QVector<QPointF> &data);
    Q_INVOKABLE double linearCoefficient(const QVector<QPointF> &data);
    Q_INVOKABLE double sigma(const QVector<double> &data);

    Q_INVOKABLE bool bitTest(int num, int bit);
    Q_INVOKABLE int bitSet(int num, int bit);
    Q_INVOKABLE int bitClear(int num, int bit);
    Q_INVOKABLE int bitToggle(int num, int bit);
};

#endif // MATHUTILS_H
