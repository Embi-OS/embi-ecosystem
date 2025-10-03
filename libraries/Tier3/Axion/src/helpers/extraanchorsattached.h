#ifndef EXTRAANCHORSATTACHED_H
#define EXTRAANCHORSATTACHED_H

#include <QDefs>
#include <QQuickItem>

class ExtraAnchorsAttached : public QObject 
{
    Q_OBJECT
    QML_ATTACHED(ExtraAnchorsAttached)
    QML_UNCREATABLE("Attached")
    QML_NAMED_ELEMENT(ExtraAnchors)
    
    /// side dock
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, topDock, TopDock, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, leftDock, LeftDock, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, rightDock, RightDock, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, bottomDock, BottomDock, nullptr)

    /// axis fill
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, verticalFill, VerticalFill, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, horizontalFill, HorizontalFill, nullptr)

    /// corner position
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, topLeftCorner, TopLeftCorner, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, topRightCorner, TopRightCorner, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, bottomLeftCorner, BottomLeftCorner, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, bottomRightCorner, BottomRightCorner, nullptr)

public:
    static ExtraAnchorsAttached* qmlAttachedProperties(QObject* object);

private:
    explicit ExtraAnchorsAttached(QObject* parent=nullptr);

    enum Sides {
        TOP,
        LEFT,
        RIGHT,
        BOTTOM,
    };

    void defineAnchorLine(QQuickItem* otherItem, const Sides side);

    QObject* m_anchors;
};

#endif // EXTRAANCHORSATTACHED_H
