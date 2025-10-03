#ifndef SCENEPOSITIONATTACHED_H
#define SCENEPOSITIONATTACHED_H

#include <QObject>
#include <QQmlEngine>
#include <QPointF>

class QQuickItem;

/**
 * This attached property contains the information about the scene position of the item:
 * Its global x and y coordinates will update automatically and can be binded
 * @code
 * import org.kde.kirigami 2.5 as Kirigami
 * Text {
 *    text: ScenePosition.x
 * }
 * @endcode
 * @since 2.3
 */
class ScenePositionAttached : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(ScenePositionAttached)
    QML_NAMED_ELEMENT(ScenePosition)
    QML_UNCREATABLE("")
    /**
     * The global scene X position
     */
    Q_PROPERTY(double x READ x NOTIFY xChanged FINAL)

    /**
     * The global scene Y position
     */
    Q_PROPERTY(double y READ y NOTIFY yChanged FINAL)

public:
    explicit ScenePositionAttached(QObject *parent = nullptr);
    ~ScenePositionAttached() override;

    double x() const;
    double y() const;

    // QML attached property
    static ScenePositionAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void xChanged();
    void yChanged();

private:
    void connectAncestors(QQuickItem *item);

    QQuickItem *m_item = nullptr;
    QList<QQuickItem *> m_ancestors;
};

QML_DECLARE_TYPEINFO(ScenePositionAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif // SCENEPOSITIONATTACHED_H
