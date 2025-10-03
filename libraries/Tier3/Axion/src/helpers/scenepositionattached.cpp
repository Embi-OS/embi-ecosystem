#include "scenepositionattached.h"
#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>

ScenePositionAttached::ScenePositionAttached(QObject *parent)
    : QObject(parent)
{
    m_item = qobject_cast<QQuickItem *>(parent);
    connectAncestors(m_item);
}

ScenePositionAttached::~ScenePositionAttached()
{
}

double ScenePositionAttached::x() const
{
    double x = 0.0;
    QQuickItem *item = m_item;

    while (item) {
        if(m_item->window() && item->parentItem()==m_item->window()->contentItem())
            break;
        x += item->x();
        item = item->parentItem();
    }

    return x;
}

double ScenePositionAttached::y() const
{
    double y = 0.0;
    QQuickItem *item = m_item;

    while (item) {
        if(m_item->window() && item->parentItem()==m_item->window()->contentItem())
            break;
        y += item->y();
        item = item->parentItem();
    }

    return y;
}

void ScenePositionAttached::connectAncestors(QQuickItem *item)
{
    if (!item) {
        return;
    }

    QQuickItem *ancestor = item;
    while (ancestor) {
        m_ancestors << ancestor;

        connect(ancestor, &QQuickItem::xChanged, this, &ScenePositionAttached::xChanged);
        connect(ancestor, &QQuickItem::yChanged, this, &ScenePositionAttached::yChanged);
        connect(ancestor, &QQuickItem::parentChanged, this, [this, ancestor]() {
            while (!m_ancestors.isEmpty()) {
                QQuickItem *last = m_ancestors.takeLast();
                // Disconnect the item which had its parent changed too,
                // because connectAncestors() would reconnect it next.
                disconnect(last, nullptr, this, nullptr);
                if (last == ancestor) {
                    break;
                }
            }

            connectAncestors(ancestor);

            Q_EMIT xChanged();
            Q_EMIT yChanged();
        });

        ancestor = ancestor->parentItem();
    }
}

ScenePositionAttached *ScenePositionAttached::qmlAttachedProperties(QObject *object)
{
    return new ScenePositionAttached(object);
}

#include "moc_scenepositionattached.cpp"
