#ifndef SLIDINGITEMIMPL_H
#define SLIDINGITEMIMPL_H

#include <QQuickItem>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

#include <QDefs>
#include "axion_helpertypes.h"

class SlidingItemImpl : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(QQuickItem, contentItem, ContentItem, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_VAR_PROPERTY(Qt::Alignment, horizontalAlignment, HorizontalAlignment, Qt::AlignLeft)
    Q_WRITABLE_VAR_PROPERTY(Qt::Alignment, verticalAlignment, VerticalAlignment, Qt::AlignLeft)
    Q_WRITABLE_VAR_PROPERTY(bool, vertical, Vertical, false)

    Q_WRITABLE_VAR_PROPERTY(int, duration, Duration, -1)
    Q_WRITABLE_REF_PROPERTY(QEasingCurve, easingCurve, EasingCurve, QEasingCurve::Linear)

    Q_WRITABLE_FUZ_PROPERTY(double, topPadding, TopPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, leftPadding, LeftPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, rightPadding, RightPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, bottomPadding, BottomPadding, 0)

    Q_READONLY_FUZ_PROPERTY(double, implicitContentWidth, ImplicitContentWidth, 0)
    Q_READONLY_FUZ_PROPERTY(double, implicitContentHeight, ImplicitContentHeight, 0)

public:
    explicit SlidingItemImpl(QQuickItem* parent = nullptr);

protected:
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    void updateImplicitContentSize();
    void layout();

    void computeContentSmaller(double availableWidth, double availableHeight);
    void computeContentWider(double availableWidth, double availableHeight);
    void computeContentTaller(double availableWidth, double availableHeight);

private slots:
    void connectToContentItem();
    void disconnectFromContentItem();

    void queueInvalidate();
    void invalidate();

private:
    bool m_complete = false;
    bool m_invalidateQueued = false;

    QSequentialAnimationGroup* m_animation=nullptr;
    QPropertyAnimation* m_animation1=nullptr;
    QPropertyAnimation* m_animation2=nullptr;
};

#endif // SLIDINGITEMIMPL_H
