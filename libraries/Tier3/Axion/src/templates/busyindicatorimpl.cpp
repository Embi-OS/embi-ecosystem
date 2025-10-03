#include "busyindicatorimpl.h"
#include "animatednode.h"

#include <QtCore/qmath.h>
#include <QtCore/qeasingcurve.h>
#include <QtGui/qpainter.h>
#include <QtQuick/qsgimagenode.h>
#include <QtQuick/qquickwindow.h>

static const int SpanAnimationDuration = 700;
static const int RotationAnimationDuration = SpanAnimationDuration * 6;
static const int TargetRotation = 720;
static const int OneDegree = 16;
static const double MinSweepSpan = 10 * OneDegree;
static const double MaxSweepSpan = 300 * OneDegree;

class BusyIndicatorNode : public AnimatedNode
{
public:
    BusyIndicatorNode(BusyIndicatorImpl *item);

    void sync(QQuickItem *item) override;

protected:
    void updateCurrentTime(int time) override;

private:
    int m_lastStartAngle = 0;
    int m_lastEndAngle = 0;
    double m_width = 0;
    double m_height = 0;
    double m_devicePixelRatio = 1;
    QColor m_color;
    QColor m_backgroundColor;
    double m_lineWidth;
    Qt::PenCapStyle m_capStyle;
};

BusyIndicatorNode::BusyIndicatorNode(BusyIndicatorImpl *item)
    : AnimatedNode(item)
{
    setLoopCount(Infinite);
    setCurrentTime(item->elapsed());
    setDuration(RotationAnimationDuration);

    QSGImageNode *textureNode = item->window()->createImageNode();
    textureNode->setOwnsTexture(true);
    appendChildNode(textureNode);

    // A texture seems to be required here, but we don't have one yet, as we haven't drawn anything,
    // so just use a blank image.
    QImage blankImage(item->width(), item->height(), QImage::Format_ARGB32_Premultiplied);
    blankImage.fill(Qt::transparent);
    textureNode->setTexture(item->window()->createTextureFromImage(blankImage));
}

void BusyIndicatorNode::updateCurrentTime(int time)
{
    const double w = m_width;
    const double h = m_height;
    const double size = qMin(w, h);
    const double dx = (w - size) / 2;
    const double dy = (h - size) / 2;

    QImage image(size * m_devicePixelRatio, size * m_devicePixelRatio, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QPen pen;
    if(m_lineWidth>0)
        pen.setWidth(m_lineWidth);
    else
        pen.setWidth((qCeil(size / 12) * m_devicePixelRatio));

    const int halfPen = pen.width() / 2;
    const QRectF arcBounds = QRectF(halfPen, halfPen,
                                    m_devicePixelRatio * size - pen.width(),
                                    m_devicePixelRatio * size - pen.width());

    // Draw the background
    pen.setColor(m_backgroundColor);
    painter.setPen(pen);
    painter.drawArc(arcBounds, 0, 360 * 16);

    // Draw the foreground
    QSGImageNode *textureNode = static_cast<QSGImageNode *>(firstChild());
    pen.setColor(m_color);
    pen.setCapStyle(m_capStyle);
    painter.setPen(pen);

    const double percentageComplete = time / double(RotationAnimationDuration);
    const double spanPercentageComplete = (time % SpanAnimationDuration) / double(SpanAnimationDuration);
    const int iteration = time / SpanAnimationDuration;
    int startAngle = 0;
    int endAngle = 0;

    if (iteration % 2 == 0) {
        if (m_lastStartAngle > 360 * OneDegree)
            m_lastStartAngle -= 360 * OneDegree;

        // The start angle is only affected by the rotation animation for the "grow" phase.
        startAngle = m_lastStartAngle;
        QEasingCurve angleCurve(QEasingCurve::OutQuad);
        const double percentage = angleCurve.valueForProgress(spanPercentageComplete);
        endAngle = m_lastStartAngle + MinSweepSpan + percentage * (MaxSweepSpan - MinSweepSpan);
        m_lastEndAngle = endAngle;
    } else {
        // Both the start angle *and* the span are affected by the "shrink" phase.
        QEasingCurve angleCurve(QEasingCurve::InQuad);
        const double percentage = angleCurve.valueForProgress(spanPercentageComplete);
        startAngle = m_lastEndAngle - MaxSweepSpan + percentage * (MaxSweepSpan - MinSweepSpan);
        endAngle = m_lastEndAngle;
        m_lastStartAngle = startAngle;
    }

    // The current angle of the rotation animation.
    const double rotation = OneDegree * percentageComplete * -TargetRotation;
    startAngle -= rotation;
    endAngle -= rotation;
    const int angleSpan = endAngle - startAngle;
    painter.drawArc(arcBounds, -startAngle, -angleSpan);
    painter.end();

    textureNode->setRect(QRectF(dx, dy, size, size));
    textureNode->setTexture(window()->createTextureFromImage(image));
}

void BusyIndicatorNode::sync(QQuickItem *item)
{
    BusyIndicatorImpl *indicator = static_cast<BusyIndicatorImpl *>(item);
    m_color = indicator->getColor();
    m_backgroundColor = indicator->getBackgroundColor();
    m_lineWidth = indicator->getLineWidth();
    m_capStyle = indicator->getCapStyle();
    m_width = indicator->width();
    m_height = indicator->height();
    m_devicePixelRatio = indicator->window()->effectiveDevicePixelRatio();
}

BusyIndicatorImpl::BusyIndicatorImpl(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(ItemHasContents);

    connect(this, &BusyIndicatorImpl::visibleChanged, this, &BusyIndicatorImpl::runningChanged);
    connect(this, &BusyIndicatorImpl::colorChanged, this, &BusyIndicatorImpl::update);
    connect(this, &BusyIndicatorImpl::backgroundColorChanged, this, &BusyIndicatorImpl::update);
    connect(this, &BusyIndicatorImpl::lineWidthChanged, this, &BusyIndicatorImpl::update);
    connect(this, &BusyIndicatorImpl::capStyleChanged, this, &BusyIndicatorImpl::update);
}

bool BusyIndicatorImpl::isRunning() const
{
    return isVisible();
}

void BusyIndicatorImpl::setRunning(bool running)
{
    if (running)
        setVisible(true);
}

int BusyIndicatorImpl::elapsed() const
{
    return m_elapsed;
}

void BusyIndicatorImpl::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    switch (change) {
    case ItemOpacityHasChanged:
        if (qFuzzyIsNull(data.realValue))
            setVisible(false);
        break;
    case ItemVisibleHasChanged:
        update();
        break;
    default:
        break;
    }
}

QSGNode *BusyIndicatorImpl::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    BusyIndicatorNode *node = static_cast<BusyIndicatorNode *>(oldNode);
    if (isRunning() && width() > 0 && height() > 0) {
        if (!node) {
            node = new BusyIndicatorNode(this);
            node->start();
        }
        node->sync(this);
    } else {
        m_elapsed = node ? node->currentTime() : 0;
        delete node;
        node = nullptr;
    }
    return node;
}
