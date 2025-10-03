#ifndef EXTRAPOSITIONATTACHED_H
#define EXTRAPOSITIONATTACHED_H

#include <QDefs>

#include <QQuickItem>
#include <QQmlProperty>

#include "axion_helpertypes.h"

class ExtraPositionAttached : public QObject
{
    Q_OBJECT
    QML_ATTACHED(ExtraPositionAttached)
    QML_UNCREATABLE("Attached")
    QML_NAMED_ELEMENT(ExtraPosition)

    Q_WRITABLE_VAR_PROPERTY(ItemPositions::Enum, position, Position, ItemPositions::None)
    Q_WRITABLE_FUZ_PROPERTY(double, margins, Margins, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, xOffset, XOffset, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, yOffset, YOffset, 0)
    Q_WRITABLE_PTR_PROPERTY(QObject, targetParent, TargetParent, nullptr)

public:
    static ExtraPositionAttached* qmlAttachedProperties(QObject* object);

private slots:
    void onTargetParentChanged();
    void scheduleRefresh();

private:
    explicit ExtraPositionAttached(QObject* parent=nullptr);

    void definePosition();
    static double defineX(ItemPosition position, const double width, const double parentWidth, const double margins=0);
    static double defineY(ItemPosition position, const double height, const double parentHeight, const double margins=0);

    QObject* m_target;
    QQmlProperty* m_xProperty;
    QQmlProperty* m_yProperty;

    QQmlProperty* m_heightProperty;
    QQmlProperty* m_widthProperty;

    QQmlProperty* m_parentHeightProperty;
    QQmlProperty* m_parentWidthProperty;

    QTimer m_inhibitTimer;
};

#endif // EXTRAPOSITIONATTACHED_H
