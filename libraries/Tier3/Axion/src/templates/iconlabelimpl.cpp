#include "iconlabelimpl.h"

IconLabelImpl::IconLabelImpl(QQuickItem* parent):
    QQuickItem(parent),
    m_icon(new IconObject(this))
{
    connect(this, &IconLabelImpl::iconChanged, this, &IconLabelImpl::connectToIcon);
    connect(this, &IconLabelImpl::iconAboutToChange, this, &IconLabelImpl::disconnectFromIcon);

    connect(this, &IconLabelImpl::iconItemChanged, this, &IconLabelImpl::connectToIconItem);
    connect(this, &IconLabelImpl::iconItemAboutToChange, this, &IconLabelImpl::disconnectFromIconItem);

    connect(this, &IconLabelImpl::labelItemChanged, this, &IconLabelImpl::connectToLabelItem);
    connect(this, &IconLabelImpl::labelItemAboutToChange, this, &IconLabelImpl::disconnectFromLabelItem);

    connect(this, &IconLabelImpl::displayChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::alignmentChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::mirroredChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::labelFillWidthChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::spacingChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::topPaddingChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::leftPaddingChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::rightPaddingChanged, this, &IconLabelImpl::queueInvalidate);
    connect(this, &IconLabelImpl::bottomPaddingChanged, this, &IconLabelImpl::queueInvalidate);

    connectToIcon();
}

void IconLabelImpl::componentComplete()
{
    m_complete = true;
    QQuickItem::componentComplete();
    invalidate();
}

void IconLabelImpl::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    layout();
}

bool IconLabelImpl::hasIcon() const
{
    return m_display != IconLabelDisplays::TextOnly && m_iconItem && m_iconItem->isVisible() && m_icon && !m_icon->isEmpty();
}

bool IconLabelImpl::hasText() const
{
    return m_display != IconLabelDisplays::IconOnly && m_labelItem && m_labelItem->isVisible() && !m_text.isEmpty();
}

void IconLabelImpl::updateImplicitContentSize()
{
    const bool showIcon = m_iconItem && hasIcon();
    const bool showText = m_labelItem && hasText();
    const double horizontalPadding = m_leftPadding + m_rightPadding;
    const double verticalPadding = m_topPadding + m_bottomPadding;
    const double iconImplicitWidth = showIcon ? m_iconItem->implicitWidth() : 0;
    const double iconImplicitHeight = showIcon ? m_iconItem->implicitHeight() : 0;
    const double textImplicitWidth = showText ? m_labelItem->implicitWidth() : 0;
    const double textImplicitHeight = showText ? m_labelItem->implicitHeight() : 0;
    const double effectiveSpacing = showText && showIcon && m_iconItem->implicitWidth() > 0 ? m_spacing : 0;
    const double implicitWidth = m_display == IconLabelDisplays::TextBesideIcon ? iconImplicitWidth + textImplicitWidth + effectiveSpacing
                                                                           : qMax(iconImplicitWidth, textImplicitWidth);
    const double implicitHeight = m_display == IconLabelDisplays::TextUnderIcon ? iconImplicitHeight + textImplicitHeight + effectiveSpacing
                                                                           : qMax(iconImplicitHeight, textImplicitHeight);

    setImplicitContentWidth(implicitWidth + horizontalPadding);
    setImplicitContentHeight(implicitHeight + verticalPadding);
}

// adapted from QStyle::alignedRect()
static QRectF alignedRect(bool mirrored, Qt::Alignment alignment, const QSizeF &size, const QRectF &rectangle)
{
    // alignment = QGuiApplicationPrivate::visualAlignment(mirrored ? Qt::RightToLeft : Qt::LeftToRight, alignment);
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

void IconLabelImpl::layout()
{
    if (!isComponentComplete())
        return;

    const bool showIcon = m_iconItem && hasIcon();
    const bool showText = m_labelItem && hasText();

    IconLabelDisplays::Enum display = m_display;

    if((display==IconLabelDisplays::TextUnderIcon || display==IconLabelDisplays::TextBesideIcon) &&
        !showIcon && showText)
        display = IconLabelDisplays::TextOnly;
    else if((display==IconLabelDisplays::TextUnderIcon || display==IconLabelDisplays::TextBesideIcon) &&
             showIcon && !showText)
        display = IconLabelDisplays::IconOnly;

    const double availableWidth = width() - m_leftPadding - m_rightPadding;
    const double availableHeight = height() - m_topPadding - m_bottomPadding;

    switch (display) {
    case IconLabelDisplays::IconOnly:
        computeIconOnlyRect(availableWidth, availableHeight);
        break;
    case IconLabelDisplays::TextOnly:
        computeTextOnlyRect(availableWidth, availableHeight);
        break;
    case IconLabelDisplays::TextUnderIcon:
        computeTextUnderIcon(availableWidth, availableHeight);
        break;
    case IconLabelDisplays::TextBesideIcon:
    default:
        computeTextBesideIcon(availableWidth, availableHeight);
        break;
    }

    setBaselineOffset(m_labelItem ? m_labelItem->y() + m_labelItem->baselineOffset() : 0);
}

void IconLabelImpl::computeIconOnlyRect(double availableWidth, double availableHeight)
{
    if (m_iconItem) {
        const QRectF iconRect = alignedRect(m_mirrored, m_alignment,
                                            QSizeF(qMin(m_iconItem->implicitWidth(), availableWidth),
                                                   qMin(m_iconItem->implicitHeight(), availableHeight)),
                                            QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_iconItem->setSize(iconRect.size());
        m_iconItem->setPosition(iconRect.topLeft());
    }
}

void IconLabelImpl::computeTextOnlyRect(double availableWidth, double availableHeight)
{
    if (m_labelItem) {
        const QRectF textRect = alignedRect(m_mirrored, m_alignment,
                                            QSizeF(qMin(m_labelItem->implicitWidth(), availableWidth),
                                                   qMin(m_labelItem->implicitHeight(), availableHeight)),
                                            QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

        m_labelItem->setSize(textRect.size());
        m_labelItem->setPosition(textRect.topLeft());
    }
}

void IconLabelImpl::computeTextBesideIcon(double availableWidth, double availableHeight)
{
    // Work out the sizes first, as the positions depend on them.
    QSizeF iconSize(0, 0);
    QSizeF textSize(0, 0);
    if (m_iconItem) {
        iconSize.setWidth(qMin(m_iconItem->implicitWidth(), availableWidth));
        iconSize.setHeight(qMin(m_iconItem->implicitHeight(), availableHeight));
    }
    double effectiveSpacing = 0;
    if (m_labelItem) {
        if (!iconSize.isEmpty())
            effectiveSpacing = m_spacing;
        if(m_labelFillWidth)
            textSize.setWidth(availableWidth - iconSize.width() - effectiveSpacing);
        else
            textSize.setWidth(qMin(m_labelItem->implicitWidth(), availableWidth - iconSize.width() - effectiveSpacing));
        textSize.setHeight(qMin(m_labelItem->implicitHeight(), availableHeight));
    }

    const QRectF combinedRect = alignedRect(m_mirrored, m_alignment,
                                            QSizeF(iconSize.width() + effectiveSpacing + textSize.width(),
                                                   qMax(iconSize.height(), textSize.height())),
                                            QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

    if (m_iconItem) {
        const QRectF iconRect = alignedRect(m_mirrored, Qt::AlignLeft | Qt::AlignVCenter, iconSize, combinedRect);
        m_iconItem->setSize(iconRect.size());
        m_iconItem->setPosition(iconRect.topLeft());
    }
    if (m_labelItem) {
        const QRectF textRect = alignedRect(m_mirrored, Qt::AlignRight | Qt::AlignVCenter, textSize, combinedRect);
        m_labelItem->setSize(textRect.size());
        m_labelItem->setPosition(textRect.topLeft());
    }
}

void IconLabelImpl::computeTextUnderIcon(double availableWidth, double availableHeight)
{
    // Work out the sizes first, as the positions depend on them.
    QSizeF iconSize;
    QSizeF textSize;
    if (m_iconItem) {
        iconSize.setWidth(qMin(m_iconItem->implicitWidth(), availableWidth));
        iconSize.setHeight(qMin(m_iconItem->implicitHeight(), availableHeight));
    }
    double effectiveSpacing = 0;
    if (m_labelItem) {
        if (!iconSize.isEmpty())
            effectiveSpacing = m_spacing;
        textSize.setWidth(qMin(m_labelItem->implicitWidth(), availableWidth));
        textSize.setHeight(qMin(m_labelItem->implicitHeight(), availableHeight - iconSize.height() - effectiveSpacing));
    }

    QRectF combinedRect = alignedRect(m_mirrored, m_alignment,
                                      QSizeF(qMax(iconSize.width(), textSize.width()),
                                             iconSize.height() + effectiveSpacing + textSize.height()),
                                      QRectF(m_leftPadding, m_topPadding, availableWidth, availableHeight));

    if (m_iconItem) {
        QRectF iconRect = alignedRect(m_mirrored, Qt::AlignHCenter | Qt::AlignTop, iconSize, combinedRect);
        m_iconItem->setSize(iconRect.size());
        m_iconItem->setPosition(iconRect.topLeft());
    }
    if (m_labelItem) {
        QRectF textRect = alignedRect(m_mirrored, Qt::AlignHCenter | Qt::AlignBottom, textSize, combinedRect);
        m_labelItem->setSize(textRect.size());
        m_labelItem->setPosition(textRect.topLeft());
    }
}

void IconLabelImpl::connectToIcon()
{
    if(m_icon)
    {
        connect(m_icon, &IconObject::sourceChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_icon, &IconObject::heightChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_icon, &IconObject::widthChanged, this, &IconLabelImpl::queueInvalidate);
    }

    queueInvalidate();
}

void IconLabelImpl::disconnectFromIcon()
{
    if(!m_icon)
        return;

    disconnect(m_icon, nullptr, this, nullptr);
    disconnect(this, nullptr, m_icon, nullptr);

    if(m_icon->parent() == this)
        m_icon->deleteLater();
}

void IconLabelImpl::connectToIconItem()
{
    if(m_iconItem)
    {
        m_iconItem->setParentItem(this);

        connect(m_iconItem, &QQuickItem::visibleChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_iconItem, &QQuickItem::implicitWidthChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_iconItem, &QQuickItem::implicitHeightChanged, this, &IconLabelImpl::queueInvalidate);
    }

    queueInvalidate();
}

void IconLabelImpl::disconnectFromIconItem()
{
    if(!m_iconItem)
        return;

    disconnect(m_iconItem, nullptr, this, nullptr);
    disconnect(this, nullptr, m_iconItem, nullptr);

    if(m_iconItem->parent() == this)
    {
        m_iconItem->setParentItem(nullptr);
        // TODO : Delete the m_iconItem somehow when qt quick will expose public api to do it correctly during object creation
    }
}

void IconLabelImpl::connectToLabelItem()
{
    if(m_labelItem)
    {
        m_labelItem->setParentItem(this);

        connect(m_labelItem, &QQuickItem::visibleChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_labelItem, &QQuickItem::implicitWidthChanged, this, &IconLabelImpl::queueInvalidate);
        connect(m_labelItem, &QQuickItem::implicitHeightChanged, this, &IconLabelImpl::queueInvalidate);
    }

    queueInvalidate();
}

void IconLabelImpl::disconnectFromLabelItem()
{
    if(!m_labelItem)
        return;

    disconnect(m_labelItem, nullptr, this, nullptr);
    disconnect(this, nullptr, m_labelItem, nullptr);

    if(m_labelItem->parent() == this)
    {
        m_labelItem->setParentItem(nullptr);
        // TODO : Delete the m_labelItem somehow when qt quick will expose public api to do it correctly during object creation
    }
}

void IconLabelImpl::queueInvalidate()
{
    if(!m_complete)
        return;

    if (m_delayed) {
        if(m_invalidateQueued)
            return;
        m_invalidateQueued = true;
        QMetaObject::invokeMethod(this, &IconLabelImpl::invalidate, Qt::QueuedConnection);
    } else {
        invalidate();
    }
}

void IconLabelImpl::invalidate()
{
    m_invalidateQueued = false;

    updateImplicitContentSize();
    layout();
}
