#include "extraanchorsattached.h"

#include <QQmlProperty>

ExtraAnchorsAttached::ExtraAnchorsAttached(QObject* parent):
    QObject (parent),
    m_anchors(parent ? QQmlProperty(parent, "anchors").read().value<QObject *>() : nullptr)
{
    connect (this, &ExtraAnchorsAttached::verticalFillChanged, [this]() {
        defineAnchorLine (m_verticalFill, TOP);
        defineAnchorLine (m_verticalFill, BOTTOM);
    });
    connect (this, &ExtraAnchorsAttached::horizontalFillChanged, [this]() {
        defineAnchorLine (m_horizontalFill, LEFT);
        defineAnchorLine (m_horizontalFill, RIGHT);
    });
    connect (this, &ExtraAnchorsAttached::topLeftCornerChanged, [this]() {
        defineAnchorLine (m_topLeftCorner, TOP);
        defineAnchorLine (m_topLeftCorner, LEFT);
    });
    connect (this, &ExtraAnchorsAttached::topRightCornerChanged, [this]() {
        defineAnchorLine (m_topRightCorner, TOP);
        defineAnchorLine (m_topRightCorner, RIGHT);
    });
    connect (this, &ExtraAnchorsAttached::bottomLeftCornerChanged, [this]() {
        defineAnchorLine (m_bottomLeftCorner, LEFT);
        defineAnchorLine (m_bottomLeftCorner, BOTTOM);
    });
    connect (this, &ExtraAnchorsAttached::bottomRightCornerChanged, [this]() {
        defineAnchorLine (m_bottomRightCorner, RIGHT);
        defineAnchorLine (m_bottomRightCorner, BOTTOM);
    });
    connect (this, &ExtraAnchorsAttached::topDockChanged, [this]() {
        defineAnchorLine (m_topDock, TOP);
        defineAnchorLine (m_topDock, LEFT);
        defineAnchorLine (m_topDock, RIGHT);
    });
    connect (this, &ExtraAnchorsAttached::leftDockChanged, [this]() {
        defineAnchorLine (m_leftDock, TOP);
        defineAnchorLine (m_leftDock, LEFT);
        defineAnchorLine (m_leftDock, BOTTOM);
    });
    connect (this, &ExtraAnchorsAttached::rightDockChanged, [this]() {
        defineAnchorLine (m_rightDock, TOP);
        defineAnchorLine (m_rightDock, RIGHT);
        defineAnchorLine (m_rightDock, BOTTOM);
    });
    connect (this, &ExtraAnchorsAttached::bottomDockChanged, [this]() {
        defineAnchorLine (m_bottomDock, LEFT);
        defineAnchorLine (m_bottomDock, RIGHT);
        defineAnchorLine (m_bottomDock, BOTTOM);
    });
}

ExtraAnchorsAttached* ExtraAnchorsAttached::qmlAttachedProperties (QObject * object)
{
    return new ExtraAnchorsAttached(object);
}

void ExtraAnchorsAttached::defineAnchorLine(QQuickItem* otherItem, const Sides side)
{
    static const QVariant UNDEFINED;
    if (m_anchors)
    {
        const QString lineName {
            [side]() -> QString {
                switch (side) {
                    case TOP:    return QStringLiteral ("top");
                    case LEFT:   return QStringLiteral ("left");
                    case RIGHT:  return QStringLiteral ("right");
                    case BOTTOM: return QStringLiteral ("bottom");
                }
                return "";
            } ()
        };
        if (!lineName.isEmpty ())
        {
            QQmlProperty prop{m_anchors, lineName};
            if(otherItem)
            {
                QQmlProperty tmp{otherItem, lineName};
                prop.write(tmp.read());
            }
            else
            {
                prop.write(UNDEFINED);
            }
        }
    }
}
