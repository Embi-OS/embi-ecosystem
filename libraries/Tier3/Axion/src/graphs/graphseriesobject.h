#ifndef GRAPHSERIESOBJECT_H
#define GRAPHSERIESOBJECT_H

#include <QXYSeries>
#include <QVector3D>

#include <QDefs>
#include <Unit>

class GraphObject;

class GraphSeriesObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_WRITABLE_REF_PROPERTY(QFont, font, Font, {})
    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, {})
    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "Main")
    Q_WRITABLE_VAR_PROPERTY(bool, useOpenGL, UseOpenGL, true)
    Q_WRITABLE_VAR_PROPERTY(bool, visible, Visible, true)
    Q_WRITABLE_VAR_PROPERTY(int, type, Type, QAbstractSeries::SeriesTypeLine)
    Q_WRITABLE_FUZ_PROPERTY(float, width, Width, 1)

    Q_READONLY_VAR_PROPERTY(QPointF, lastPointRaw, LastPointRaw, {})
    Q_PROPERTY(QPointF lastPoint READ getLastPoint NOTIFY throttled FINAL)

    Q_READONLY_REF_PROPERTY(QPointF, xGlobalMaxRaw, XGlobalMaxRaw, {})
    Q_READONLY_REF_PROPERTY(QPointF, xGlobalMinRaw, XGlobalMinRaw, {})
    Q_PROPERTY(QPointF xGlobalMax READ getXGlobalMax NOTIFY throttled FINAL)
    Q_PROPERTY(QPointF xGlobalMin READ getXGlobalMin NOTIFY throttled FINAL)

    Q_READONLY_REF_PROPERTY(QPointF, yGlobalMaxRaw, YGlobalMaxRaw, {})
    Q_READONLY_REF_PROPERTY(QPointF, yGlobalMinRaw, YGlobalMinRaw, {})
    Q_PROPERTY(QPointF yGlobalMax READ getYGlobalMax NOTIFY throttled FINAL)
    Q_PROPERTY(QPointF yGlobalMin READ getYGlobalMin NOTIFY throttled FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, maxPlottedPoints, MaxPlottedPoints, 0)

    Q_CONSTANT_PTR_PROPERTY(QGenericSignalThrottler, throttler, Throttler, nullptr)

    Q_WRITABLE_REF_PROPERTY(QVector<QVector3D>, pointsRaw, PointsRaw, {})
    Q_READONLY_REF_PROPERTY(QVector<QVector3D>, points, Points, {})
    Q_READONLY_REF_PROPERTY(QVector<QPointF>, displayPoints, DisplayPoints, {})
    Q_READONLY_VAR_PROPERTY(int, pointsCount, PointsCount, 0)

public:
    explicit GraphSeriesObject(QObject* parent = nullptr);

    void reset();
    void resetMinMax();

    QPointF getXGlobalMax() const;
    QPointF getXGlobalMin() const;
    QPointF getYGlobalMax() const;
    QPointF getYGlobalMin() const;
    QPointF getLastPoint() const;

    void changePointsRaw(const QVector<QVector3D>& pointsRaw);

public slots:
    bool addPoint(const QPointF& point, qreal zValue=0);
    void updateChartSeries(QAbstractSeries *abstractSeries);

signals:
    void throttled();

private slots:
    bool updateMinMax(const QVector3D& point);
    bool processPoint(const QVector3D& point);
    void addPlotPoint(const QVector3D& pointRaw);

private:
    GraphObject* m_graph;
};

#endif // GRAPHSERIESOBJECT_H
