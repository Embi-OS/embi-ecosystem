#include "labelwithcaptionimpl.h"

LabelWithCaptionImpl::LabelWithCaptionImpl(QQuickItem* parent):
    QQuickItem(parent)
{
    connect(this, &LabelWithCaptionImpl::labelItemChanged, this, &LabelWithCaptionImpl::connectToLabelItem);
    connect(this, &LabelWithCaptionImpl::labelItemAboutToChange, this, &LabelWithCaptionImpl::disconnectFromLabelItem);

    connect(this, &LabelWithCaptionImpl::captionItemChanged, this, &LabelWithCaptionImpl::connectToCaptionItem);
    connect(this, &LabelWithCaptionImpl::captionItemAboutToChange, this, &LabelWithCaptionImpl::disconnectFromCaptionItem);

    connect(this, &LabelWithCaptionImpl::haveATextChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::haveACaptionChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::horizontalAlignmentChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::verticalAlignmentChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::spacingChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::topPaddingChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::leftPaddingChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::rightPaddingChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    connect(this, &LabelWithCaptionImpl::bottomPaddingChanged, this, &LabelWithCaptionImpl::queueInvalidate);

}

void LabelWithCaptionImpl::componentComplete()
{
    m_complete = true;
    QQuickItem::componentComplete();
    invalidate();
}

void LabelWithCaptionImpl::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    layout();
}

bool LabelWithCaptionImpl::hasText() const
{
    return m_labelItem && m_labelItem->isVisible() && (m_haveAText || !m_text.isEmpty());
}

bool LabelWithCaptionImpl::hasCaption() const
{
    return m_captionItem && m_captionItem->isVisible() && (m_haveACaption || !m_caption.isEmpty());
}

void LabelWithCaptionImpl::updateImplicitContentSize()
{
    const bool showLabel = m_labelItem && hasText();
    const bool showCaption = m_captionItem && hasCaption();
    const double labelImplicitWidth = showLabel ? m_labelItem->implicitWidth() : 0;
    const double labelImplicitHeight = showLabel ? m_labelItem->implicitHeight() : 0;
    const double captionImplicitWidth = showCaption ? m_captionItem->implicitWidth() : 0;
    const double captionImplicitHeight = showCaption ? m_captionItem->implicitHeight() : 0;
    const double effectiveSpacing = showLabel && showCaption ? m_spacing : 0;

    const double horizontalPadding = m_leftPadding + m_rightPadding;
    const double verticalPadding = m_topPadding + m_bottomPadding;
    const double implicitWidth = qMax(labelImplicitWidth, captionImplicitWidth);
    const double implicitHeight = labelImplicitHeight + captionImplicitHeight + effectiveSpacing;

    setImplicitContentWidth(implicitWidth + horizontalPadding);
    setImplicitContentHeight(implicitHeight + verticalPadding);
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

void LabelWithCaptionImpl::layout()
{
    if (!isComponentComplete())
        return;

    const bool showText = m_labelItem && hasText();
    const bool showCaption = m_captionItem && hasCaption();

    const double availableWidth = width() - m_leftPadding - m_rightPadding;
    const double availableHeight = height() - m_topPadding - m_bottomPadding;

    if(showText && showCaption) {
        computeTextCaptionRect(availableWidth, availableHeight);
    }
    else if(showText) {
        computeTextOnlyRect(availableWidth, availableHeight);
    }
    else if(showCaption) {
        computeCaptionOnlyRect(availableWidth, availableHeight);
    }
    else {
        computeEmpty(availableWidth, availableHeight);
    }

    setBaselineOffset(m_labelItem ? m_labelItem->y() + m_labelItem->baselineOffset() : 0);
}

void LabelWithCaptionImpl::computeTextOnlyRect(double availableWidth, double availableHeight)
{
    if (m_labelItem) {
        const QRectF labelRect = alignedRect(m_horizontalAlignment | m_verticalAlignment,
                                             QSizeF(qMin(m_labelItem->implicitWidth(), availableWidth),
                                                    qMin(m_labelItem->implicitHeight(), availableHeight)),
                                             QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_labelItem->setSize(labelRect.size());
        m_labelItem->setPosition(labelRect.topLeft());
    }
}

void LabelWithCaptionImpl::computeCaptionOnlyRect(double availableWidth, double availableHeight)
{
    if (m_captionItem) {
        const QRectF captionRect = alignedRect(m_horizontalAlignment | m_verticalAlignment,
                                               QSizeF(qMin(m_captionItem->implicitWidth(), availableWidth),
                                                      qMin(m_captionItem->implicitHeight(), availableHeight)),
                                               QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_captionItem->setSize(captionRect.size());
        m_captionItem->setPosition(captionRect.topLeft());
    }
}

void LabelWithCaptionImpl::computeTextCaptionRect(double availableWidth, double availableHeight)
{
    // Work out the sizes first, as the positions depend on them.
    QSizeF labelSize;
    QSizeF captionSize;
    if (m_labelItem) {
        labelSize.setWidth(qMin(m_labelItem->implicitWidth(), availableWidth));
        labelSize.setHeight(qMin(m_labelItem->implicitHeight(), availableHeight));
    }
    double effectiveSpacing = 0;
    if (m_captionItem) {
        if (!labelSize.isEmpty())
            effectiveSpacing = m_spacing;
        captionSize.setWidth(qMin(m_captionItem->implicitWidth(), availableWidth));
        captionSize.setHeight(qMin(m_captionItem->implicitHeight(), availableHeight - labelSize.height() - effectiveSpacing));
    }

    QRectF combinedRect = alignedRect(m_horizontalAlignment | m_verticalAlignment,
                                      QSizeF(qMax(labelSize.width(), captionSize.width()),
                                             labelSize.height() + effectiveSpacing + captionSize.height()),
                                      QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

    if (m_labelItem) {
        QRectF labelRect = alignedRect(m_horizontalAlignment | Qt::AlignTop, labelSize, combinedRect);
        m_labelItem->setSize(labelRect.size());
        m_labelItem->setPosition(labelRect.topLeft());
    }
    if (m_captionItem) {
        QRectF captionRect = alignedRect(m_horizontalAlignment | Qt::AlignBottom, captionSize, combinedRect);
        m_captionItem->setSize(captionRect.size());
        m_captionItem->setPosition(captionRect.topLeft());
    }
}

void LabelWithCaptionImpl::computeEmpty(double availableWidth, double availableHeight)
{

}

void LabelWithCaptionImpl::connectToLabelItem()
{
    if(m_labelItem)
    {
        m_labelItem->setParentItem(this);

        connect(m_labelItem, &QQuickItem::visibleChanged, this, &LabelWithCaptionImpl::queueInvalidate);
        connect(m_labelItem, &QQuickItem::implicitWidthChanged, this, &LabelWithCaptionImpl::queueInvalidate);
        connect(m_labelItem, &QQuickItem::implicitHeightChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    }

    queueInvalidate();
}

void LabelWithCaptionImpl::disconnectFromLabelItem()
{
    if(!m_labelItem)
        return;

    disconnect(m_labelItem, nullptr, this, nullptr);
    disconnect(this, nullptr, m_labelItem, nullptr);

    if(m_labelItem->parent() == this)
    {
        m_labelItem->setParentItem(nullptr);
        // TODO : Delete the m_iconItem somehow when qt quick will expose public api to do it correctly during object creation
    }
}

void LabelWithCaptionImpl::connectToCaptionItem()
{
    if(m_captionItem)
    {
        m_captionItem->setParentItem(this);

        connect(m_captionItem, &QQuickItem::visibleChanged, this, &LabelWithCaptionImpl::queueInvalidate);
        connect(m_captionItem, &QQuickItem::implicitWidthChanged, this, &LabelWithCaptionImpl::queueInvalidate);
        connect(m_captionItem, &QQuickItem::implicitHeightChanged, this, &LabelWithCaptionImpl::queueInvalidate);
    }

    queueInvalidate();
}

void LabelWithCaptionImpl::disconnectFromCaptionItem()
{
    if(!m_captionItem)
        return;

    disconnect(m_captionItem, nullptr, this, nullptr);
    disconnect(this, nullptr, m_captionItem, nullptr);

    if(m_captionItem->parent() == this)
    {
        m_captionItem->setParentItem(nullptr);
        // TODO : Delete the m_iconItem somehow when qt quick will expose public api to do it correctly during object creation
    }
}

void LabelWithCaptionImpl::queueInvalidate()
{
    if(!m_complete)
        return;

    if (m_delayed) {
        if(m_invalidateQueued)
            return;
        m_invalidateQueued = true;
        QMetaObject::invokeMethod(this, &LabelWithCaptionImpl::invalidate, Qt::QueuedConnection);
    } else {
        invalidate();
    }
}

void LabelWithCaptionImpl::invalidate()
{
    m_invalidateQueued = false;

    updateImplicitContentSize();
    layout();
}
