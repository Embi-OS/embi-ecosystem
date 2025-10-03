#include "extrapositionattached.h"
#include "axion_log.h"

ExtraPositionAttached::ExtraPositionAttached(QObject* parent):
    QObject (parent),
    m_targetParent(nullptr),
    m_target(parent),
    m_xProperty(new QQmlProperty(parent, "x")),
    m_yProperty(new QQmlProperty(parent, "y")),
    m_heightProperty(new QQmlProperty(parent, "height")),
    m_widthProperty(new QQmlProperty(parent, "width")),
    m_parentHeightProperty(nullptr),
    m_parentWidthProperty(nullptr),
    m_inhibitTimer(this)
{
    connect(this, &ExtraPositionAttached::targetParentChanged, this, &ExtraPositionAttached::onTargetParentChanged);

    m_inhibitTimer.setInterval(0);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &ExtraPositionAttached::definePosition, Qt::UniqueConnection);

    connect(this, &ExtraPositionAttached::positionChanged, this, &ExtraPositionAttached::scheduleRefresh);
    connect(this, &ExtraPositionAttached::marginsChanged, this, &ExtraPositionAttached::scheduleRefresh);
    connect(this, &ExtraPositionAttached::xOffsetChanged, this, &ExtraPositionAttached::scheduleRefresh);
    connect(this, &ExtraPositionAttached::yOffsetChanged, this, &ExtraPositionAttached::scheduleRefresh);

    m_heightProperty->connectNotifySignal(this, SLOT(scheduleRefresh()));
    m_widthProperty->connectNotifySignal(this, SLOT(scheduleRefresh()));

    scheduleRefresh();
}

ExtraPositionAttached* ExtraPositionAttached::qmlAttachedProperties (QObject * object)
{
    return new ExtraPositionAttached(object);
}

void ExtraPositionAttached::onTargetParentChanged()
{
    if(m_parentHeightProperty)
        delete m_parentHeightProperty;
    if(m_parentWidthProperty)
        delete m_parentWidthProperty;

    if(!m_targetParent)
        return;

    m_parentHeightProperty = new QQmlProperty(m_targetParent, "height"),
    m_parentWidthProperty = new QQmlProperty(m_targetParent, "width"),

    m_parentHeightProperty->connectNotifySignal(this, SLOT(scheduleRefresh()));
    m_parentWidthProperty->connectNotifySignal(this, SLOT(scheduleRefresh()));
}

void ExtraPositionAttached::scheduleRefresh()
{
    m_inhibitTimer.stop();
    m_inhibitTimer.start();
}

void ExtraPositionAttached::definePosition()
{
    if(!m_targetParent)
        setTargetParent(QQmlProperty(m_target, "parent").read().value<QObject *>());

    static const QVariant UNDEFINED;

    if (!m_targetParent || m_position==ItemPositions::None)
    {
        m_xProperty->write(UNDEFINED);
        m_yProperty->write(UNDEFINED);
        return;
    }

    const double width = m_widthProperty->read().toReal();
    const double parentWidth = m_parentWidthProperty->read().toReal();
    const double x = defineX(m_position, width, parentWidth, m_margins) + m_xOffset;
    m_xProperty->write(x);

    const double height = m_heightProperty->read().toReal();
    const double parentHeight = m_parentHeightProperty->read().toReal();
    const double y = defineY(m_position, height, parentHeight, m_margins) + m_yOffset;
    m_yProperty->write(y);
}

double ExtraPositionAttached::defineX(ItemPosition position, const double width, const double parentWidth, const double margins)
{
    switch (position) {
    case ItemPositions::BottomLeft:
    case ItemPositions::TopLeft:
        return -width - margins / 2;
    case ItemPositions::BottomStart:
    case ItemPositions::TopStart:
        return 0;
    case ItemPositions::Bottom:
    case ItemPositions::Center:
    case ItemPositions::Top:
        return (parentWidth - width) / 2;
    case ItemPositions::BottomEnd:
    case ItemPositions::TopEnd:
        return parentWidth - width;
    case ItemPositions::BottomRight:
    case ItemPositions::TopRight:
        return parentWidth + margins / 2;
    case ItemPositions::RightStart:
    case ItemPositions::Right:
    case ItemPositions::RightEnd:
        return parentWidth + margins / 2;
    case ItemPositions::LeftEnd:
    case ItemPositions::Left:
    case ItemPositions::LeftStart:
        return -width - margins / 2;
    default:
        AXIONLOG_WARNING()<<"Unsupported Position:"<<position;
        return 0;
    }
}

double ExtraPositionAttached::defineY(ItemPosition position, const double height, const double parentHeight, const double margins)
{
    switch (position) {
    case ItemPositions::TopLeft:
    case ItemPositions::TopStart:
    case ItemPositions::Top:
    case ItemPositions::TopEnd:
    case ItemPositions::TopRight:
        return -height - margins / 2;
    case ItemPositions::BottomLeft:
    case ItemPositions::BottomStart:
    case ItemPositions::Bottom:
    case ItemPositions::BottomEnd:
    case ItemPositions::BottomRight:
        return parentHeight + margins / 2;
    case ItemPositions::RightStart:
    case ItemPositions::LeftStart:
        return 0;
    case ItemPositions::Right:
    case ItemPositions::Center:
    case ItemPositions::Left:
        return (parentHeight - height) / 2;
    case ItemPositions::RightEnd:
    case ItemPositions::LeftEnd:
        return parentHeight - height;
    default:
        AXIONLOG_WARNING()<<"Unsupported Position:"<<position;
        return 0;
    }
}
