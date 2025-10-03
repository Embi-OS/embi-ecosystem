#include "circulargaugeimpl.h"

#include <QPainter>

CircularGaugeImpl::CircularGaugeImpl(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setHeight(100);
    setWidth(100);
    setSmooth(true);
    setAntialiasing(true);

    connect(this, &CircularGaugeImpl::valueChanged, this, &QQuickItem::update);
}

void CircularGaugeImpl::paint(QPainter *painter)
{
    double size = qMin(width(), height());
    double xPos = (width()-size)/2;
    double yPos = (height()-size)/2;
    QRectF rect = QRectF(xPos, yPos, size, size);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    QPen pen = painter->pen();
    pen.setCapStyle(m_capStyle);

    double valueBounded = qMin(qMax(m_value, m_from), m_to);
    double offset = m_lineWidth / 2;
    double startAngle = - (m_startAngle+90) * 16;
    double spanAngle = - (m_spanAngle) * 16;
    double valueAngle = - (((valueBounded - m_from)/(m_to - m_from)) * m_spanAngle) * 16;  //Map value to angle range
    QRectF rectAdjusted = rect.adjusted(offset, offset, -offset, -offset);

    double radius = rectAdjusted.width()/2;
    double xCenter = xPos + size/2;
    double yCenter = yPos + size/2;

    //Draw text
    if(m_showText && !m_text.isEmpty())
    {
        const QRectF adjustedRect = rect.adjusted(m_lineWidth*1.5, m_lineWidth*1.5, -m_lineWidth*1.5, -m_lineWidth*1.5);
        painter->save();

        // Ajustement dynamique de la taille du texte
        const QFont textFont = ::adjustedFontForRect(m_text, m_textFont, adjustedRect, painter);
        QRectF textRect;

        painter->setFont(textFont);
        pen.setColor(m_textColor);
        painter->setPen(pen);
        painter->drawText(adjustedRect, Qt::AlignCenter, m_text, &textRect);
        painter->restore();


        //Draw caption
        if(m_showCaption && !m_caption.isEmpty())
        {
            const QRectF captionRect = QRectF(textRect.left()+2, textRect.bottom(), textRect.width()-4, adjustedRect.bottom()-textRect.bottom());
            painter->save();

            const QFont captionFont = ::adjustedFontForRect(m_caption, m_captionFont, captionRect, painter);

            painter->setFont(captionFont);
            pen.setColor(m_captionColor);
            painter->setPen(pen);
            painter->drawText(captionRect, Qt::AlignHCenter | Qt::AlignTop, m_caption);
            painter->restore();
        }
    }

    //Draw bounds
    if(m_showBounds)
    {
        double boundSize = m_lineWidth*0.8;
        double boundWidth = boundSize/3.0;
        bool drawLowerBond = m_value<m_from || (m_showMarble && m_marbleValue<m_from);
        bool drawUpperBond = m_value>m_to || (m_showMarble && m_marbleValue>m_to);

        painter->save();
        if(drawLowerBond)
        {
            double angle = qDegreesToRadians(m_startAngle+90);
            double dotx = radius * qCos(angle);
            double doty = radius * qSin(angle);
            painter->translate(xCenter+dotx, yCenter+doty);
            double rotation = (qRadiansToDegrees(angle)+45+180);
            painter->rotate(rotation);
            painter->fillRect(boundWidth, boundWidth, boundSize, boundSize, m_progressColor);
            painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
            painter->fillRect(0, 0, boundSize, boundSize, Qt::transparent);
        }
        else if(drawUpperBond)
        {
            double angle = qDegreesToRadians(m_startAngle+90+m_spanAngle);
            double dotx = radius * qCos(angle);
            double doty = radius * qSin(angle);
            painter->translate(xCenter+dotx, yCenter+doty);
            double rotation = (qRadiansToDegrees(angle)+45);
            painter->rotate(rotation);
            painter->fillRect(boundWidth, boundWidth, boundSize, boundSize, m_progressColor);
            painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
            painter->fillRect(0, 0, boundSize, boundSize, Qt::transparent);
        }
        painter->restore();
    }

    //Draw Background
    painter->save();
    pen.setWidth(m_lineWidth);
    pen.setColor(m_backgroundColor);
    painter->setPen(pen);
    painter->drawArc(rectAdjusted, startAngle, spanAngle);
    painter->restore();

    //Draw Progress
    painter->save();
    pen.setWidth(m_lineWidth);
    pen.setColor(m_progressColor);
    painter->setPen(pen);
    painter->drawArc(rectAdjusted, startAngle, valueAngle);
    painter->restore();

    //Draw marble
    if(m_showMarble)
    {
        if(m_marbleValue>=m_from && m_marbleValue<=m_to)
        {
            double marbleAngle = (((m_marbleValue - m_from)/(m_to - m_from)) * m_spanAngle + (m_startAngle+90));
            double angle = qDegreesToRadians(marbleAngle);
            double rotation = qRadiansToDegrees(angle);
            double size = m_lineWidth-2;

            painter->save();
            painter->translate(xCenter, yCenter);
            painter->rotate(rotation);
            painter->translate(radius, 0);
            pen.setWidth(0);
            pen.setColor(m_marbleColor);
            painter->setPen(pen);
            painter->setBrush(m_marbleColor);
            painter->drawRoundedRect(-size/2, -size/2, size, size, size/2, size/2);
            painter->restore();
        }
    }

    //Draw ticks
    if(m_showTicks && !m_ticks.isEmpty())
    {
        double tickHeight = m_lineWidth/2;
        double tickWidth = m_lineWidth+2;

        for(const double& tick: std::as_const(m_ticks))
        {
            if(tick<m_from || tick>m_to)
                continue;

            double tickAngle = (((tick - m_from)/(m_to - m_from)) * m_spanAngle + (m_startAngle+90));
            double angle = qDegreesToRadians(tickAngle);
            double rotation = qRadiansToDegrees(angle);

            painter->save();
            painter->translate(xCenter, yCenter);
            painter->rotate(rotation);
            painter->translate(radius, 0);
            painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
            painter->fillRect(-tickWidth/2, -tickHeight/2, tickWidth, tickHeight, Qt::transparent);
            painter->restore();
        }
    }
}
