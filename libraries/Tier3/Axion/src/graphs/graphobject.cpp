#include "graphobject.h"
#include "graph_log.h"

GraphObject::GraphObject(UnitCategory xCategory, UnitCategory yCategory, QObject *parent) :
    QObject(parent),
    m_xAxis(new GraphAxisObject(xCategory, this)),
    m_yAxis(new GraphAxisObject(yCategory, this)),
    m_mainSeries(new GraphSeriesObject(this)),
    m_series(new GraphSeriesModel(this))
{
    addSeries(m_mainSeries);
}

void GraphObject::reset()
{
    m_series->reset();

    m_xAxis->reset();
    m_yAxis->reset();

    resetBounds();
    resetMinMax();
}

void GraphObject::resetBounds()
{
    m_xAxis->resetBounds();
    m_yAxis->resetBounds();
}

void GraphObject::resetMinMax()
{
    m_xAxis->resetMinMax();
    m_yAxis->resetMinMax();

    m_series->resetMinMax();
}

bool GraphObject::addPoint(const QPointF& point, int seriesIndex, qreal zValue)
{
    if(seriesIndex<0 || seriesIndex>=m_series->count())
    {
        GRAPHLOG_WARNING()<<"Cannot add point to this series -> index:"<<seriesIndex;
        return false;
    }

    m_xAxis->addValue(point.x());
    m_yAxis->addValue(point.y());

    bool result =  m_series->at<GraphSeriesObject>(seriesIndex)->addPoint(point, zValue);

    if(result)
        emit this->pointAdded();

    return result;
}

QPointF GraphObject::convertRawPoint(const QPointF& pointRaw) const
{
    return getConverterFunction()(pointRaw);
}

GraphConvertFunction GraphObject::getConverterFunction() const
{
    const float xRatio = Units::make(m_xAxis->category())->ratio(m_xAxis->getUnit());
    const float yRatio = Units::make(m_yAxis->category())->ratio(m_yAxis->getUnit());

    GraphConvertFunction f = [xRatio, yRatio](const QPointF& pointRaw) {
        return QPointF(pointRaw.x()*xRatio, pointRaw.y()*yRatio);
    };

    return f;
}

void GraphObject::addSeries(GraphSeriesObject* series)
{
    m_series->append(series);
}
