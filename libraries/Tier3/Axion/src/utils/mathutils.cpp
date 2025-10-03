#include "mathutils.h"

MathUtils::MathUtils(QObject *parent) :
    QObject(parent)
{

}

double MathUtils::abs(double val) const
{
    return ::qAbs(val);
}

double MathUtils::round(double val) const
{
    return ::qRound(val);
}

double MathUtils::sign(double val) const
{
    return ::qSign(val);
}

double MathUtils::bound(double min, double val, double max) const
{
    return ::qBound(min, val, max);
}

double MathUtils::fuzzyModulo(double p1, double p2, double precision) const
{
    return ::qFuzzyModulo(p1, p2, precision);
}

bool MathUtils::fuzzyCompare(double p1, double p2, double precision) const
{
    return ::qFuzzyCompare(p1, p2, precision);
}

bool MathUtils::fuzzyIsNull(double d, double precision) const
{
    return ::qFuzzyIsNull(d, precision);
}

QPointF MathUtils::linearRegression(const QVector<QPointF> &data)
{
    return ::qLinearRegression(data);
}

double MathUtils::linearCoefficient(const QVector<QPointF> &data)
{
    return ::qLinearCoefficient(data);
}

double MathUtils::sigma(const QVector<double> &data)
{
    return ::qSigma(data);
}

bool MathUtils::bitTest(int num, int bit)
{
    return (num >> bit) & 1;
}

int MathUtils::bitSet(int num, int bit)
{
    return num | (1 << bit);
}

int MathUtils::bitClear(int num, int bit)
{
    return num & ~(1 << bit);
}

int MathUtils::bitToggle(int num, int bit)
{
    return bitTest(num, bit) ? bitClear(num, bit) : bitSet(num, bit);
}
