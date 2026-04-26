#include "gameoflifeview.h"

#include "gameoflifemodel.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleRectNode>

namespace
{
constexpr int verticesPerCell = 6;
constexpr int maxCellsPerGeometryNode = 10000;

struct GridMetrics
{
    bool valid;
    bool drawGrid;
    qreal cellSize;
    qreal spacing;
    qreal step;
    qreal cellAreaWidth;
    qreal cellAreaHeight;
    qreal gridAreaWidth;
    qreal gridAreaHeight;
    qreal gridOriginX;
    qreal gridOriginY;
    qreal originX;
    qreal originY;
};

QSGGeometryNode *createCellGeometryNode()
{
    auto *geometryNode = new QSGGeometryNode;
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
    geometry->setDrawingMode(QSGGeometry::DrawTriangles);
    geometryNode->setGeometry(geometry);
    geometryNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new QSGFlatColorMaterial;
    geometryNode->setMaterial(material);
    geometryNode->setFlag(QSGNode::OwnsMaterial);

    return geometryNode;
}

void ensureGeometryNodeCount(QSGNode *container, int targetCount)
{
    int currentCount = 0;
    for (QSGNode *child = container->firstChild(); child; child = child->nextSibling())
        ++currentCount;

    while (currentCount < targetCount) {
        container->appendChildNode(createCellGeometryNode());
        ++currentCount;
    }

    while (currentCount > targetCount) {
        delete container->lastChild();
        --currentCount;
    }
}

GridMetrics gridMetricsForView(qreal itemWidth, qreal itemHeight, int rowCount, int columnCount, qreal cellSize, qreal cellSpacing)
{
    GridMetrics metrics {};
    metrics.valid = rowCount > 0 && columnCount > 0 && cellSize > 0.0;
    if (!metrics.valid)
        return metrics;

    metrics.drawGrid = cellSpacing > 0.0;
    metrics.cellSize = cellSize;
    metrics.spacing = cellSpacing;
    metrics.step = cellSize + cellSpacing;
    metrics.cellAreaWidth = columnCount * cellSize + (columnCount - 1) * cellSpacing;
    metrics.cellAreaHeight = rowCount * cellSize + (rowCount - 1) * cellSpacing;
    metrics.gridAreaWidth = metrics.drawGrid ? metrics.cellAreaWidth + 2.0 * cellSpacing : metrics.cellAreaWidth;
    metrics.gridAreaHeight = metrics.drawGrid ? metrics.cellAreaHeight + 2.0 * cellSpacing : metrics.cellAreaHeight;
    metrics.gridOriginX = qMax<qreal>(0.0, (itemWidth - metrics.gridAreaWidth) * 0.5);
    metrics.gridOriginY = qMax<qreal>(0.0, (itemHeight - metrics.gridAreaHeight) * 0.5);
    metrics.originX = metrics.drawGrid ? metrics.gridOriginX + cellSpacing : metrics.gridOriginX;
    metrics.originY = metrics.drawGrid ? metrics.gridOriginY + cellSpacing : metrics.gridOriginY;
    return metrics;
}
}

GameOfLifeView::GameOfLifeView(QQuickItem *parent):
    QQuickItem(parent),
    m_rowCount(0),
    m_columnCount(0),
    m_isPainting(false),
    m_paintAlive(true),
    m_lastPaintedRow(-1),
    m_lastPaintedColumn(-1)
{
    setFlag(ItemHasContents);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

    connect(this, &GameOfLifeView::cellSizeChanged, this, &QQuickItem::update);
    connect(this, &GameOfLifeView::cellSpacingChanged, this, &QQuickItem::update);
    connect(this, &GameOfLifeView::backgroundColorChanged, this, &QQuickItem::update);
    connect(this, &GameOfLifeView::gridColorChanged, this, &QQuickItem::update);
    connect(this, &GameOfLifeView::cellColorChanged, this, &QQuickItem::update);

    connect(this, &GameOfLifeView::modelAboutToChange, this, [this](GameOfLifeModel *oldModel, GameOfLifeModel *) {
        if(oldModel) {
            disconnect(oldModel, nullptr, this, nullptr);
        }
    });
    connect(this, &GameOfLifeView::modelChanged, this, [this](GameOfLifeModel *model) {
        if (model) {
            connect(model, &GameOfLifeModel::boardChanged, this, &GameOfLifeView::scheduleModelSync);
            connect(model, &QAbstractItemModel::modelReset, this, &GameOfLifeView::scheduleModelSync);
        }

        scheduleModelSync();
    });
}

void GameOfLifeView::updatePolish()
{
    syncModelState();
}

QSGNode *GameOfLifeView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    QElapsedTimer timer;
    timer.start();

    QSGNode *rootNode = oldNode;
    if (!rootNode) {
        rootNode = new QSGNode;
        rootNode->appendChildNode(new QSGSimpleRectNode(QRectF(), m_backgroundColor));
        rootNode->appendChildNode(new QSGSimpleRectNode(QRectF(), m_gridColor));
        rootNode->appendChildNode(new QSGNode);
        rootNode->appendChildNode(new QSGNode);
    }

    auto *backgroundNode = static_cast<QSGSimpleRectNode *>(rootNode->firstChild());
    auto *gridNode = static_cast<QSGSimpleRectNode *>(backgroundNode->nextSibling());
    auto *backgroundCellsRoot = gridNode->nextSibling();
    auto *aliveCellsRoot = backgroundCellsRoot->nextSibling();

    const bool drawGrid = m_cellSpacing > 0.0;
    const bool drawBackgroundCells = drawGrid && m_backgroundColor != m_gridColor;

    backgroundNode->setRect(boundingRect());
    backgroundNode->setColor(m_backgroundColor);

    if (m_rowCount <= 0 || m_columnCount <= 0 || m_cellSize <= 0.0) {
        gridNode->setRect(QRectF());
        ensureGeometryNodeCount(backgroundCellsRoot, 0);
        ensureGeometryNodeCount(aliveCellsRoot, 0);
        return rootNode;
    }

    const int totalCellCount = m_rowCount * m_columnCount;
    const int backgroundChunkCount = drawBackgroundCells
                                         ? (totalCellCount + maxCellsPerGeometryNode - 1) / maxCellsPerGeometryNode
                                         : 0;
    const int aliveChunkCount = (m_aliveCellSnapshot.size() + maxCellsPerGeometryNode - 1) / maxCellsPerGeometryNode;

    ensureGeometryNodeCount(backgroundCellsRoot, backgroundChunkCount);
    ensureGeometryNodeCount(aliveCellsRoot, aliveChunkCount);

    for (QSGNode *child = backgroundCellsRoot->firstChild(); child; child = child->nextSibling()) {
        auto *geometryNode = static_cast<QSGGeometryNode *>(child);
        auto *material = static_cast<QSGFlatColorMaterial *>(geometryNode->material());
        material->setColor(m_backgroundColor);
        geometryNode->markDirty(QSGNode::DirtyMaterial);
    }

    for (QSGNode *child = aliveCellsRoot->firstChild(); child; child = child->nextSibling()) {
        auto *geometryNode = static_cast<QSGGeometryNode *>(child);
        auto *material = static_cast<QSGFlatColorMaterial *>(geometryNode->material());
        material->setColor(m_cellColor);
        geometryNode->markDirty(QSGNode::DirtyMaterial);
    }

    int remainingBackgroundCells = totalCellCount;
    for (QSGNode *child = backgroundCellsRoot->firstChild(); child; child = child->nextSibling()) {
        auto *geometryNode = static_cast<QSGGeometryNode *>(child);
        auto *geometry = geometryNode->geometry();
        const int chunkCellCount = qMin(remainingBackgroundCells, maxCellsPerGeometryNode);
        geometry->allocate(chunkCellCount * verticesPerCell);
        geometryNode->markDirty(QSGNode::DirtyGeometry);
        remainingBackgroundCells -= chunkCellCount;
    }

    int remainingAliveCells = m_aliveCellSnapshot.size();
    for (QSGNode *child = aliveCellsRoot->firstChild(); child; child = child->nextSibling()) {
        auto *geometryNode = static_cast<QSGGeometryNode *>(child);
        auto *geometry = geometryNode->geometry();
        const int chunkCellCount = qMin(remainingAliveCells, maxCellsPerGeometryNode);
        geometry->allocate(chunkCellCount * verticesPerCell);
        geometryNode->markDirty(QSGNode::DirtyGeometry);
        remainingAliveCells -= chunkCellCount;
    }

    auto *backgroundNodeGeometry = static_cast<QSGGeometryNode *>(backgroundCellsRoot->firstChild());
    auto *aliveNode = static_cast<QSGGeometryNode *>(aliveCellsRoot->firstChild());
    auto *backgroundVertices = backgroundNodeGeometry ? backgroundNodeGeometry->geometry()->vertexDataAsPoint2D() : nullptr;
    auto *aliveVertices = aliveNode ? aliveNode->geometry()->vertexDataAsPoint2D() : nullptr;
    int backgroundCellsInChunk = 0;
    int aliveCellsInChunk = 0;

    const GridMetrics metrics = gridMetricsForView(width(), height(), m_rowCount, m_columnCount, m_cellSize, m_cellSpacing);
    const float cellSize = float(metrics.cellSize);
    const float step = float(metrics.step);
    const float originX = float(metrics.originX);
    const float originY = float(metrics.originY);

    gridNode->setColor(m_gridColor);
    gridNode->setRect(drawGrid ? QRectF(metrics.gridOriginX, metrics.gridOriginY, metrics.gridAreaWidth, metrics.gridAreaHeight) : QRectF());

    if (drawBackgroundCells) {
        for (int row = 0; row < m_rowCount; ++row) {
            const float y0 = originY + float(row) * step;
            const float y1 = y0 + cellSize;

            for (int column = 0; column < m_columnCount; ++column) {
                const float x0 = originX + float(column) * step;
                const float x1 = x0 + cellSize;

                backgroundVertices[0].set(x0, y0);
                backgroundVertices[1].set(x1, y0);
                backgroundVertices[2].set(x0, y1);
                backgroundVertices[3].set(x1, y0);
                backgroundVertices[4].set(x1, y1);
                backgroundVertices[5].set(x0, y1);
                backgroundVertices += verticesPerCell;
                ++backgroundCellsInChunk;

                if (backgroundCellsInChunk == maxCellsPerGeometryNode) {
                    backgroundNodeGeometry = static_cast<QSGGeometryNode *>(backgroundNodeGeometry->nextSibling());
                    backgroundVertices = backgroundNodeGeometry ? backgroundNodeGeometry->geometry()->vertexDataAsPoint2D() : nullptr;
                    backgroundCellsInChunk = 0;
                }
            }
        }
    }

    for (int aliveCell : m_aliveCellSnapshot) {
        const int row = aliveCell / m_columnCount;
        const int column = aliveCell % m_columnCount;
        const float x0 = originX + float(column) * step;
        const float y0 = originY + float(row) * step;
        const float x1 = x0 + cellSize;
        const float y1 = y0 + cellSize;

        aliveVertices[0].set(x0, y0);
        aliveVertices[1].set(x1, y0);
        aliveVertices[2].set(x0, y1);
        aliveVertices[3].set(x1, y0);
        aliveVertices[4].set(x1, y1);
        aliveVertices[5].set(x0, y1);
        aliveVertices += verticesPerCell;
        ++aliveCellsInChunk;

        if (aliveCellsInChunk == maxCellsPerGeometryNode) {
            aliveNode = static_cast<QSGGeometryNode *>(aliveNode->nextSibling());
            aliveVertices = aliveNode ? aliveNode->geometry()->vertexDataAsPoint2D() : nullptr;
            aliveCellsInChunk = 0;
        }
    }

    qDebug()<<"updatePaintNode"<<timer.nsecsElapsed()/1000000.0<<m_rowCount<<m_columnCount;

    return rootNode;
}

void GameOfLifeView::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    update();
}

void GameOfLifeView::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() != Qt::LeftButton && event->button() != Qt::RightButton) || !m_model) {
        event->ignore();
        return;
    }

    m_isPainting = true;
    m_paintAlive = event->button() == Qt::LeftButton;
    setKeepMouseGrab(true);
    m_lastPaintedRow = -1;
    m_lastPaintedColumn = -1;

    int row = -1;
    int column = -1;
    if (cellAtPosition(event->position(), row, column))
        paintStroke(row, column);

    event->accept();
}

void GameOfLifeView::mouseMoveEvent(QMouseEvent *event)
{
    const Qt::MouseButton activeButton = m_paintAlive ? Qt::LeftButton : Qt::RightButton;
    if (!m_isPainting || !(event->buttons() & activeButton) || !m_model) {
        event->ignore();
        return;
    }

    int row = -1;
    int column = -1;
    if (cellAtPosition(event->position(), row, column))
        paintStroke(row, column);

    event->accept();
}

void GameOfLifeView::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton && m_paintAlive) ||
        (event->button() == Qt::RightButton && !m_paintAlive)) {
        resetStroke();
        event->accept();
        return;
    }

    event->ignore();
}

void GameOfLifeView::scheduleModelSync()
{
    polish();
    update();
}

void GameOfLifeView::syncModelState()
{
    if (!m_model) {
        m_aliveCellSnapshot.clear();
        m_rowCount = 0;
        m_columnCount = 0;
        resetStroke();
        return;
    }

    m_rowCount = m_model->rowCount();
    m_columnCount = m_model->columnCount();
    m_aliveCellSnapshot = m_model->aliveCells();
}

bool GameOfLifeView::cellAtPosition(const QPointF &position, int &row, int &column) const
{
    const GridMetrics metrics = gridMetricsForView(width(), height(), m_rowCount, m_columnCount, m_cellSize, m_cellSpacing);
    if (!metrics.valid)
        return false;

    const qreal localX = position.x() - metrics.originX;
    const qreal localY = position.y() - metrics.originY;
    if (localX < 0.0 || localY < 0.0 || localX >= metrics.cellAreaWidth || localY >= metrics.cellAreaHeight)
        return false;

    column = int(localX / metrics.step);
    row = int(localY / metrics.step);
    if (row < 0 || row >= m_rowCount || column < 0 || column >= m_columnCount)
        return false;

    const qreal cellOffsetX = localX - column * metrics.step;
    const qreal cellOffsetY = localY - row * metrics.step;
    return cellOffsetX < metrics.cellSize && cellOffsetY < metrics.cellSize;
}

void GameOfLifeView::paintStroke(int row, int column)
{
    if (!m_model)
        return;

    if (m_lastPaintedRow < 0 || m_lastPaintedColumn < 0) {
        m_model->setValue(row, column, m_paintAlive);
        m_lastPaintedRow = row;
        m_lastPaintedColumn = column;
        return;
    }

    int currentRow = m_lastPaintedRow;
    int currentColumn = m_lastPaintedColumn;
    const int deltaX = qAbs(column - currentColumn);
    const int stepX = currentColumn < column ? 1 : -1;
    const int deltaY = -qAbs(row - currentRow);
    const int stepY = currentRow < row ? 1 : -1;
    int error = deltaX + deltaY;

    while (true) {
        m_model->setValue(currentRow, currentColumn, m_paintAlive);
        if (currentRow == row && currentColumn == column)
            break;

        const int doubledError = 2 * error;
        if (doubledError >= deltaY) {
            error += deltaY;
            currentColumn += stepX;
        }
        if (doubledError <= deltaX) {
            error += deltaX;
            currentRow += stepY;
        }
    }

    m_lastPaintedRow = row;
    m_lastPaintedColumn = column;
}

void GameOfLifeView::resetStroke()
{
    m_isPainting = false;
    m_paintAlive = true;
    m_lastPaintedRow = -1;
    m_lastPaintedColumn = -1;
    setKeepMouseGrab(false);
}
