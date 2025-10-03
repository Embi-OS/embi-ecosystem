#include "graphseriesobject.h"
#include "graphobject.h"
#include "graph_log.h"

#include "axion_helpertypes.h"

#include <QLineSeries>
#include <QScatterSeries>

GraphSeriesObject::GraphSeriesObject(QObject *parent) :
    QObject(parent),
    m_throttler(new QSignalTrailingThrottler(this)),
    m_graph(qobject_cast<GraphObject*>(parent))
{
    m_font = Theme::Get()->getTextTheme()->body2;
    m_color = Theme::Get()->getColorVariant();

    if (!m_graph)
    {
        GRAPHLOG_CRITICAL()<<parent;
        GRAPHLOG_FATAL("GraphSeriesObject must be attached to a GraphObject*");
    }

    m_throttler->setTimeout(32);
    connect(this, &GraphSeriesObject::xGlobalMaxRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphSeriesObject::xGlobalMinRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphSeriesObject::yGlobalMaxRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphSeriesObject::yGlobalMinRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(this, &GraphSeriesObject::lastPointRawChanged, m_throttler, &QGenericSignalThrottler::throttle);
    connect(m_throttler, &QGenericSignalThrottler::triggered, this, &GraphSeriesObject::throttled);
}

void GraphSeriesObject::reset()
{
    m_pointsRaw.clear();
    m_points.clear();
    m_displayPoints.clear();
    setPointsCount(0);

    setMaxPlottedPoints(0);
    setLastPointRaw(QPointF());

    resetMinMax();
}

void GraphSeriesObject::resetMinMax()
{
    setXGlobalMaxRaw(QPointF());
    setXGlobalMinRaw(QPointF());
    setYGlobalMaxRaw(QPointF());
    setYGlobalMinRaw(QPointF());
}

QPointF GraphSeriesObject::getXGlobalMax() const
{
    return m_graph->convertRawPoint(getXGlobalMaxRaw());
}

QPointF GraphSeriesObject::getXGlobalMin() const
{
    return m_graph->convertRawPoint(getXGlobalMinRaw());
}

QPointF GraphSeriesObject::getYGlobalMax() const
{
    return m_graph->convertRawPoint(getYGlobalMaxRaw());
}

QPointF GraphSeriesObject::getYGlobalMin() const
{
    return m_graph->convertRawPoint(getYGlobalMinRaw());
}

QPointF GraphSeriesObject::getLastPoint() const
{
    return m_graph->convertRawPoint(getLastPointRaw());
}

bool GraphSeriesObject::addPoint(const QPointF& point, qreal zValue)
{
    setLastPointRaw(point);

    QVector3D point3d = QVector3D(point.x(), point.y(), zValue);

    updateMinMax(point3d);
    return processPoint(point3d);
}

void GraphSeriesObject::updateChartSeries(QAbstractSeries *abstractSeries)
{
    if(!abstractSeries)
        return;

    abstractSeries->setName(m_name);
    abstractSeries->setUseOpenGL(m_useOpenGL);
    abstractSeries->setVisible(m_visible);

    if(!m_visible)
        return;

    switch (m_type) {
        case QAbstractSeries::SeriesTypeLine: {
            QLineSeries *series = qobject_cast<QLineSeries*>(abstractSeries);
            series->setBestFitLineVisible(true);
            series->setBestFitLineColor(Qt::white);

            QPen p = series->pen();
            p.setWidthF(m_width);
            p.setColor(m_color);
            series->setPen(p);

            series->replace(m_displayPoints);
            break;
        }
        case QAbstractSeries::SeriesTypeArea:
            break;
        case QAbstractSeries::SeriesTypeStackedBar:
            break;
        case QAbstractSeries::SeriesTypePercentBar:
            break;
        case QAbstractSeries::SeriesTypeBar:
            break;
        case QAbstractSeries::SeriesTypeHorizontalBar:
            break;
        case QAbstractSeries::SeriesTypeHorizontalPercentBar:
            break;
        case QAbstractSeries::SeriesTypeHorizontalStackedBar:
            break;
        case QAbstractSeries::SeriesTypeBoxPlot:
            break;
        case QAbstractSeries::SeriesTypeCandlestick:
            break;
        case QAbstractSeries::SeriesTypePie:
            break;
        case QAbstractSeries::SeriesTypeScatter: {
            QScatterSeries *series = qobject_cast<QScatterSeries*>(abstractSeries);
            series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
            series->setMarkerSize(m_width); // 15
            series->setColor(m_color);

            QPen p = series->pen();
            p.setWidthF(0);
            p.setColor(m_color);
            series->setPen(p);

            series->setPointLabelsFont(m_font);
            series->setPointLabelsColor(Qt::white);
            series->setPointLabelsVisible(true);
            series->setPointLabelsFormat("@yPoint");

            series->replace(m_displayPoints);
            break;
        }
        case QAbstractSeries::SeriesTypeSpline:
            break;
        default:
            GRAPHLOG_WARNING()<<"Illegal series type";
    }
}

bool GraphSeriesObject::updateMinMax(const QVector3D& point)
{
    bool ret = false;
    if(m_graph->getXAxis()->getIsAbleToMinMax())
    {
        if(getXGlobalMaxRaw().isNull() || point.x()>getXGlobalMaxRaw().x())
        {
            setXGlobalMaxRaw(point.toPointF());
            ret = true;
        }

        if(getXGlobalMinRaw().isNull() || point.x()<getXGlobalMinRaw().x())
        {
            setXGlobalMinRaw(point.toPointF());
            ret = true;
        }
    }

    if(m_graph->getYAxis()->getIsAbleToMinMax())
    {
        if(getYGlobalMaxRaw().isNull() || point.y()>getYGlobalMaxRaw().y())
        {
            setYGlobalMaxRaw(point.toPointF());
            ret = true;
        }

        if(getYGlobalMinRaw().isNull() || point.y()<getYGlobalMinRaw().y())
        {
            setYGlobalMinRaw(point.toPointF());
            ret = true;
        }
    }

    return ret;
}

bool GraphSeriesObject::processPoint(const QVector3D& point)
{
    if(m_pointsRaw.isEmpty())
    {
        addPlotPoint(point);
    }
    else if(m_pointsRaw.last().toPointF()==point.toPointF())
    {
        return false;
    }
    else
    {
        float deltaX = ABS(m_pointsRaw.last().x() - point.x());
        float deltaY = ABS(m_pointsRaw.last().y() - point.y());

        bool accepted=false;

        if(!qFuzzyIsNull(m_graph->getXAxis()->getAliasingValue()) &&
           !qFuzzyIsNull(m_graph->getYAxis()->getAliasingValue()))
        {
            accepted = (deltaX>ABS(m_graph->getXAxis()->getAliasingValue())) ||
                       (deltaY>ABS(m_graph->getYAxis()->getAliasingValue()));
        }
        else if(!qFuzzyIsNull(m_graph->getXAxis()->getAliasingValue()) &&
                qFuzzyIsNull(m_graph->getYAxis()->getAliasingValue()))
        {
            accepted = (deltaX>ABS(m_graph->getXAxis()->getAliasingValue()));
        }
        else if(qFuzzyIsNull(m_graph->getXAxis()->getAliasingValue()) &&
                !qFuzzyIsNull(m_graph->getYAxis()->getAliasingValue()))
        {
            accepted = (deltaY>ABS(m_graph->getYAxis()->getAliasingValue()));
        }
        else
        {
            accepted = true;
        }

        if(accepted)
        {
            addPlotPoint(point);
        }
        else
        {
            return false;
        }
    }

    return true;
}

void GraphSeriesObject::addPlotPoint(const QVector3D& pointRaw)
{
    QPointF point = m_graph->convertRawPoint(pointRaw.toPointF());

    m_pointsRaw.append(pointRaw);
    m_points.append(QVector3D(point.x(), point.y(), pointRaw.z()));
    m_displayPoints.append(point);

    if(m_maxPlottedPoints>0)
    {
        if(m_pointsRaw.size() > m_maxPlottedPoints)
            m_pointsRaw.remove(0, m_pointsRaw.size()-m_maxPlottedPoints);
        if(m_points.size() > m_maxPlottedPoints)
            m_points.remove(0, m_points.size()-m_maxPlottedPoints);
        if(m_displayPoints.size() > m_maxPlottedPoints)
            m_displayPoints.remove(0, m_displayPoints.size()-m_maxPlottedPoints);
    }

    emit this->pointsRawChanged(m_pointsRaw);
    emit this->pointsChanged(m_points);
    emit this->displayPointsChanged(m_displayPoints);

    setPointsCount(m_pointsRaw.count());
}

void GraphSeriesObject::changePointsRaw(const QVector<QVector3D>& pointsRaw)
{
    QElapsedTimer timer;
    timer.start();

    QVector<QVector3D> points;
    QVector<QPointF> displayPoints;

    int count = pointsRaw.size();

    points.reserve(count);
    displayPoints.reserve(count);

    GraphConvertFunction convFunc = m_graph->getConverterFunction();
    for(const QVector3D &vector3D: pointsRaw)
    {
        const QPointF point = convFunc(vector3D.toPointF());
        points.append(QVector3D(point.x(), point.y(), vector3D.z()));
        displayPoints.append(point);
    }

    setPointsRaw(pointsRaw);
    setPoints(points);
    setDisplayPoints(displayPoints);

    setPointsCount(count);

    GRAPHLOG_TRACE()<<m_name<<count<<timer.nsecsElapsed()/1000000.0;
}
