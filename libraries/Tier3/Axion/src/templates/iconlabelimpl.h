#ifndef ICONLABELIMPL_H
#define ICONLABELIMPL_H

#include <QQuickItem>

#include <QDefs>
#include "axion_helpertypes.h"

Q_ENUM_CLASS(IconLabelDisplays, IconLabelDisplay,
             IconOnly,
             TextOnly,
             TextBesideIcon,
             TextUnderIcon)

class IconLabelImpl : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(QQuickItem, iconItem, IconItem, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, labelItem, LabelItem, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_PTR_PROPERTY(IconObject, icon, Icon, nullptr)
    Q_WRITABLE_REF_PROPERTY(QString, text, Text, {})
    Q_WRITABLE_REF_PROPERTY(QFont, font, Font, {})
    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, {})
    Q_WRITABLE_VAR_PROPERTY(IconLabelDisplays::Enum, display, Display, IconLabelDisplays::IconOnly)
    Q_WRITABLE_VAR_PROPERTY(Qt::Alignment, alignment, Alignment, Qt::AlignCenter)
    Q_WRITABLE_VAR_PROPERTY(bool, labelFillWidth, LabelFillWidth, false)
    Q_WRITABLE_VAR_PROPERTY(bool, mirrored, Mirrored, false)

    Q_WRITABLE_FUZ_PROPERTY(double, spacing, Spacing, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, topPadding, TopPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, leftPadding, LeftPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, rightPadding, RightPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, bottomPadding, BottomPadding, 0)

    Q_READONLY_FUZ_PROPERTY(double, implicitContentWidth, ImplicitContentWidth, 0)
    Q_READONLY_FUZ_PROPERTY(double, implicitContentHeight, ImplicitContentHeight, 0)

public:
    explicit IconLabelImpl(QQuickItem* parent = nullptr);

protected:
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    bool hasIcon() const;
    bool hasText() const;

    void updateImplicitContentSize();
    void layout();

    void computeIconOnlyRect(double availableWidth, double availableHeight);
    void computeTextOnlyRect(double availableWidth, double availableHeight);
    void computeTextBesideIcon(double availableWidth, double availableHeight);
    void computeTextUnderIcon(double availableWidth, double availableHeight);

private slots:
    void connectToIcon();
    void disconnectFromIcon();

    void connectToIconItem();
    void disconnectFromIconItem();

    void connectToLabelItem();
    void disconnectFromLabelItem();

    void queueInvalidate();
    void invalidate();

private:
    bool m_complete = false;
    bool m_invalidateQueued = false;
};

#endif // ICONLABELIMPL_H
