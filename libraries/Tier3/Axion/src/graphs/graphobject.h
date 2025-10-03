#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <QDefs>
#include "graphaxisobject.h"
#include "graphseriesmodel.h"

typedef std::function<QPointF(const QPointF&)> GraphConvertFunction;

class GraphObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_PTR_PROPERTY(GraphAxisObject, xAxis, XAxis, nullptr)
    Q_CONSTANT_PTR_PROPERTY(GraphAxisObject, yAxis, YAxis, nullptr)
    Q_CONSTANT_PTR_PROPERTY(GraphSeriesObject, mainSeries, MainSeries, nullptr)
    Q_CONSTANT_PTR_PROPERTY(GraphSeriesModel, series, Series, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, useSeries, UseSeries, true)

public:
    explicit GraphObject(UnitCategory xCategory, UnitCategory yCategory, QObject* parent = nullptr);

    void reset();
    void resetBounds();
    void resetMinMax();

    Q_INVOKABLE QPointF convertRawPoint(const QPointF& pointRaw) const;
    GraphConvertFunction getConverterFunction() const;

    void addSeries(GraphSeriesObject* series);

public slots:
    bool addPoint(const QPointF& point, int seriesIndex=0, qreal zValue=0);

signals:
    void pointAdded();
};

#endif // GRAPHOBJECT_H
