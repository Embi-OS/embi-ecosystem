#include "slidingitemimpl.h"

SlidingItemImpl::SlidingItemImpl(QQuickItem* parent):
    QQuickItem(parent),
    m_animation(new QSequentialAnimationGroup(this)),
    m_animation1(new QPropertyAnimation(this)),
    m_animation2(new QPropertyAnimation(this))
{
    m_animation->addPause(500);
    m_animation->addAnimation(m_animation1);
    m_animation->addPause(500);
    m_animation->addAnimation(m_animation2);

    connect(this, &SlidingItemImpl::contentItemChanged, this, &SlidingItemImpl::connectToContentItem);
    connect(this, &SlidingItemImpl::contentItemAboutToChange, this, &SlidingItemImpl::disconnectFromContentItem);

    connect(this, &SlidingItemImpl::durationChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::easingCurveChanged, this, &SlidingItemImpl::queueInvalidate);

    connect(this, &SlidingItemImpl::horizontalAlignmentChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::verticalAlignmentChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::verticalChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::topPaddingChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::leftPaddingChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::rightPaddingChanged, this, &SlidingItemImpl::queueInvalidate);
    connect(this, &SlidingItemImpl::bottomPaddingChanged, this, &SlidingItemImpl::queueInvalidate);
}

void SlidingItemImpl::componentComplete()
{
    m_complete = true;
    QQuickItem::componentComplete();
    invalidate();
}

void SlidingItemImpl::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    layout();
}

void SlidingItemImpl::updateImplicitContentSize()
{
    setImplicitContentWidth(m_contentItem ? m_contentItem->implicitWidth() : 0);
    setImplicitContentHeight(m_contentItem ? m_contentItem->implicitHeight() : 0);
}

// adapted from QStyle::alignedRect()
static QRectF alignedRect(Qt::Alignment alignment, const QSizeF &size, const QRectF &rectangle)
{
    double x = rectangle.x();
    double y = rectangle.y();
    const double w = size.width();
    const double h = size.height();
    if ((alignment & Qt::AlignVCenter) == Qt::AlignVCenter)
        y += rectangle.height() / 2 - h / 2;
    else if ((alignment & Qt::AlignBottom) == Qt::AlignBottom)
        y += rectangle.height() - h;
    if ((alignment & Qt::AlignRight) == Qt::AlignRight)
        x += rectangle.width() - w;
    else if ((alignment & Qt::AlignHCenter) == Qt::AlignHCenter)
        x += rectangle.width() / 2 - w / 2;
    return QRectF(x, y, w, h);
}

void SlidingItemImpl::layout()
{
    if (!isComponentComplete())
        return;

    m_animation->stop();

    if(!m_contentItem)
        return;

    const double availableWidth = width() - m_leftPadding - m_rightPadding;
    const double availableHeight = height() - m_topPadding - m_bottomPadding;

    if(m_contentItem->implicitWidth()<=availableWidth && m_contentItem->implicitHeight()<=availableHeight)
        computeContentSmaller(availableWidth, availableHeight);
    else if(m_vertical || m_contentItem->implicitHeight()>availableHeight)
        computeContentTaller(availableWidth, availableHeight);
    else
        computeContentWider(availableWidth, availableHeight);
}

void SlidingItemImpl::computeContentSmaller(double availableWidth, double availableHeight)
{
    if (m_contentItem) {
        const QRectF contentRect = alignedRect(m_horizontalAlignment | m_verticalAlignment,
                                               QSizeF(qMin(m_contentItem->implicitWidth(), availableWidth),
                                                      qMin(m_contentItem->implicitHeight(), availableHeight)),
                                               QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_contentItem->setSize(contentRect.size());
        m_contentItem->setPosition(contentRect.topLeft());
    }
}

void SlidingItemImpl::computeContentTaller(double availableWidth, double availableHeight)
{
    if (m_contentItem) {
        const QRectF contentRect = alignedRect(Qt::AlignTop | m_horizontalAlignment,
                                               QSizeF(qMin(m_contentItem->implicitWidth(), availableWidth), m_contentItem->implicitHeight()),
                                               QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_contentItem->setSize(contentRect.size());
        m_contentItem->setPosition(contentRect.topLeft());

        m_animation1->setTargetObject(m_contentItem);
        m_animation1->setPropertyName("y");
        m_animation1->setDuration(m_duration<0 ? (m_contentItem->height()-availableHeight)*37.5 : m_duration);
        m_animation1->setEasingCurve(m_easingCurve);
        m_animation1->setStartValue(m_topPadding);
        m_animation1->setEndValue(availableHeight-m_contentItem->height()+m_topPadding);

        m_animation2->setTargetObject(m_contentItem);
        m_animation2->setPropertyName("y");
        m_animation2->setDuration(m_duration<0 ? (m_contentItem->height()-availableHeight)*37.5 : m_duration);
        m_animation2->setEasingCurve(m_easingCurve);
        m_animation2->setStartValue(availableHeight-m_contentItem->height()+m_topPadding);
        m_animation2->setEndValue(m_topPadding);

        m_animation->setLoopCount(-1);
        m_animation->start();
    }
}

void SlidingItemImpl::computeContentWider(double availableWidth, double availableHeight)
{
    if (m_contentItem) {
        const QRectF contentRect = alignedRect(m_verticalAlignment | Qt::AlignLeft,
                                               QSizeF(m_contentItem->implicitWidth(), qMin(m_contentItem->implicitHeight(), availableHeight)),
                                               QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_contentItem->setSize(contentRect.size());
        m_contentItem->setPosition(contentRect.topLeft());

        m_animation1->setTargetObject(m_contentItem);
        m_animation1->setPropertyName("x");
        m_animation1->setDuration(m_duration<0 ? (m_contentItem->width()-availableWidth)*37.5 : m_duration);
        m_animation1->setEasingCurve(m_easingCurve);
        m_animation1->setStartValue(m_leftPadding);
        m_animation1->setEndValue(availableWidth-m_contentItem->width()+m_leftPadding);

        m_animation2->setTargetObject(m_contentItem);
        m_animation2->setPropertyName("x");
        m_animation2->setDuration(m_duration<0 ? (m_contentItem->width()-availableWidth)*37.5 : m_duration);
        m_animation2->setEasingCurve(m_easingCurve);
        m_animation2->setStartValue(availableWidth-m_contentItem->width()+m_leftPadding);
        m_animation2->setEndValue(m_leftPadding);

        m_animation->setLoopCount(-1);
        m_animation->start();
    }
}

void SlidingItemImpl::connectToContentItem()
{
    if(m_contentItem)
    {
        m_contentItem->setParentItem(this);

        connect(m_contentItem, &QQuickItem::visibleChanged, this, &SlidingItemImpl::queueInvalidate);
        connect(m_contentItem, &QQuickItem::implicitWidthChanged, this, &SlidingItemImpl::queueInvalidate);
        connect(m_contentItem, &QQuickItem::implicitHeightChanged, this, &SlidingItemImpl::queueInvalidate);
    }

    queueInvalidate();
}

void SlidingItemImpl::disconnectFromContentItem()
{
    if(!m_contentItem)
        return;

    disconnect(m_contentItem, nullptr, this, nullptr);
    disconnect(this, nullptr, m_contentItem, nullptr);

    if(m_contentItem->parent() == this)
    {
        m_contentItem->setParentItem(nullptr);
        // TODO : Delete the m_contentItem somehow when qt quick will expose public api to do it correctly during object creation
    }
}

void SlidingItemImpl::queueInvalidate()
{
    if(!m_complete)
        return;

    if (m_delayed) {
        if(m_invalidateQueued)
            return;
        m_invalidateQueued = true;
        QMetaObject::invokeMethod(this, &SlidingItemImpl::invalidate, Qt::QueuedConnection);
    } else {
        invalidate();
    }
}

void SlidingItemImpl::invalidate()
{
    m_invalidateQueued = false;

    updateImplicitContentSize();
    layout();
}
