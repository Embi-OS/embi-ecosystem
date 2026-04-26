#ifndef GAMEOFLIFEVIEW_H
#define GAMEOFLIFEVIEW_H

#include <QDefs>
#include <QColor>
#include <QQuickItem>

#include "gameoflifemodel.h"

class QMouseEvent;

class GameOfLifeView : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(GameOfLifeModel, model, Model, nullptr)
    Q_WRITABLE_FUZ_PROPERTY(qreal, cellSize, CellSize, 10.0)
    Q_WRITABLE_FUZ_PROPERTY(qreal, cellSpacing, CellSpacing, 2.0)
    Q_WRITABLE_REF_PROPERTY(QColor, backgroundColor, BackgroundColor, 0x09102B)
    Q_WRITABLE_REF_PROPERTY(QColor, gridColor, GridColor, 0x09102B)
    Q_WRITABLE_REF_PROPERTY(QColor, cellColor, CellColor, Qt::black)

public:
    explicit GameOfLifeView(QQuickItem *parent = nullptr);

protected:
    void updatePolish() override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void scheduleModelSync();
    void syncModelState();

private:
    bool cellAtPosition(const QPointF &position, int &row, int &column) const;
    void paintStroke(int row, int column);
    void resetStroke();

    QVector<int> m_aliveCellSnapshot;
    int m_rowCount;
    int m_columnCount;
    bool m_isPainting;
    bool m_paintAlive;
    int m_lastPaintedRow;
    int m_lastPaintedColumn;
};

#endif // GAMEOFLIFEVIEW_H
