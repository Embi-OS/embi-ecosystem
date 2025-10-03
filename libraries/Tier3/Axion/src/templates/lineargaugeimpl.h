#ifndef LINEARGAUGEIMPL_H
#define LINEARGAUGEIMPL_H

#include <QDefs>
#include <QQuickPaintedItem>

class LinearGaugeImpl : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_FUZ_PROPERTY(double, from, From, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, to, To, 20)
    Q_WRITABLE_FUZ_PROPERTY(double, value, Value, 0)
    Q_WRITABLE_VAR_PROPERTY(bool, showBounds, ShowBounds, true)

    Q_WRITABLE_VAR_PROPERTY(double, lineWidth, LineWidth, 10)
    Q_WRITABLE_REF_PROPERTY(QColor, backgroundColor, BackgroundColor, Qt::black)
    Q_WRITABLE_REF_PROPERTY(QColor, progressColor, ProgressColor, Qt::white)
    Q_WRITABLE_VAR_PROPERTY(Qt::PenCapStyle, capStyle, CapStyle, Qt::RoundCap)
    Q_WRITABLE_FUZ_PROPERTY(double, spacing, Spacing, 5)

    Q_WRITABLE_REF_PROPERTY(QString, text, Text, {})
    Q_WRITABLE_VAR_PROPERTY(bool, showText, ShowText, true)
    Q_WRITABLE_REF_PROPERTY(QColor, textColor, TextColor, Qt::white)
    Q_WRITABLE_REF_PROPERTY(QFont, textFont, TextFont, {})

    Q_WRITABLE_REF_PROPERTY(QString, caption, Caption, {})
    Q_WRITABLE_VAR_PROPERTY(bool, showCaption, ShowCaption, false)
    Q_WRITABLE_REF_PROPERTY(QColor, captionColor, CaptionColor, Qt::white)
    Q_WRITABLE_REF_PROPERTY(QFont, captionFont, CaptionFont, {})

    Q_WRITABLE_REF_PROPERTY(QList<double>, ticks, Ticks, {})
    Q_WRITABLE_VAR_PROPERTY(bool, showTicks, ShowTicks, true)
    Q_WRITABLE_REF_PROPERTY(QColor, tickColor, TickColor, Qt::white)
    Q_WRITABLE_REF_PROPERTY(QFont, tickFont, TickFont, {})
    Q_WRITABLE_VAR_PROPERTY(int, tickDecimal, TickDecimal, 3)
    Q_WRITABLE_REF_PROPERTY(QList<double>, ticksPos, TicksPos, {})

    Q_WRITABLE_REF_PROPERTY(double, marbleValue, MarbleValue, 0)
    Q_WRITABLE_VAR_PROPERTY(bool, showMarble, ShowMarble, false)
    Q_WRITABLE_REF_PROPERTY(QColor, marbleColor, MarbleColor, Qt::black)

public:
    explicit LinearGaugeImpl(QQuickItem *parent = 0);

    void paint(QPainter *painter);
};

#endif // LINEARGAUGEIMPL_H
