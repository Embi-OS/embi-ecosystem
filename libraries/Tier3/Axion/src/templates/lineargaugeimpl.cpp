#include "lineargaugeimpl.h"

#include <QPainter>

LinearGaugeImpl::LinearGaugeImpl(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setHeight(300);
    setWidth(100);
    setSmooth(true);
    setAntialiasing(true);

    connect(this, &LinearGaugeImpl::valueChanged, this, &QQuickItem::update);
}

void LinearGaugeImpl::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    QPen pen = painter->pen();
    pen.setCapStyle(m_capStyle);

    double textsOffset = 2;

    //Draw text
    if(m_showText && !m_text.isEmpty())
    {
        textsOffset += m_textFont.pixelSize();
        const QRectF adjustedRect = QRectF(0, 0, width(), m_textFont.pixelSize());
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
            textsOffset += m_captionFont.pixelSize();
            const QRectF captionRect = QRectF(0, textRect.bottom(), width(), m_captionFont.pixelSize());
            painter->save();

            const QFont captionFont = ::adjustedFontForRect(m_caption, m_captionFont, captionRect, painter);

            painter->setFont(captionFont);
            pen.setColor(m_captionColor);
            painter->setPen(pen);
            painter->drawText(captionRect, Qt::AlignHCenter | Qt::AlignTop, m_caption);
            painter->restore();
        }
    }

    double boundSize = m_lineWidth*0.8;
    double boundWidth = boundSize/3.0;
    double boundTotalHeight = qSqrt(qPow(boundSize,2)*2)+qSqrt(qPow(boundWidth,2)*2);
    double valueBounded = qMin(qMax(m_value, m_from), m_to);
    double maxHeight = height()-textsOffset-boundTotalHeight*2;
    double valueHeight = (((valueBounded - m_from)/(m_to - m_from)) * maxHeight);

    //Draw bounds
    if(m_showBounds)
    {
        bool drawTopBound = m_value<m_from || (m_showMarble && m_marbleValue<m_from);
        bool drawBottomBound = m_value>m_to || (m_showMarble && m_marbleValue>m_to);

        painter->save();
        if(drawTopBound)
        {
            painter->translate(width()/2, textsOffset);
            double rotation = 45;
            painter->rotate(rotation);
            painter->fillRect(0, 0, boundSize, boundSize, m_progressColor);
            painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
            painter->fillRect(boundWidth, boundWidth, boundSize, boundSize, Qt::transparent);
        }
        else if(drawBottomBound)
        {
            painter->translate(width()/2, height()-boundSize*2);
            double rotation = 45;
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
    painter->drawLine(width()/2, textsOffset+boundTotalHeight, width()/2, height()-boundTotalHeight);
    painter->restore();

    //Draw Progress
    painter->save();
    pen.setWidth(m_lineWidth);
    pen.setColor(m_progressColor);
    painter->setPen(pen);
    painter->drawLine(width()/2, textsOffset+boundTotalHeight, width()/2, textsOffset+boundTotalHeight+valueHeight);
    painter->restore();

    //Draw marble
    if(m_showMarble)
    {
        if(m_marbleValue>=m_from && m_marbleValue<=m_to)
        {
            double marblePosY = (((m_marbleValue - m_from)/(m_to - m_from)) * maxHeight);
            QPointF marblePos(width()/2, textsOffset+boundTotalHeight+marblePosY);
            double size = m_lineWidth-2;

            painter->save();
            painter->translate(marblePos);
            pen.setWidth(0);
            pen.setColor(m_marbleColor);
            painter->setPen(pen);
            painter->setBrush(m_marbleColor);
            painter->drawRoundedRect(-size/2, -size/2, size, size, size/2, size/2);
            painter->restore();
        }
    }

    //Draw ticks
    QList<double> ticksPos;
    if(m_showTicks && !m_ticks.isEmpty())
    {
        double tickHeight = m_lineWidth/2;
        double tickWidth = m_lineWidth+2;

        for(const double& tick: std::as_const(m_ticks))
        {
            if(tick<m_from || tick>m_to)
                continue;

            double tickPosY = (((tick - m_from)/(m_to - m_from)) * maxHeight);
            QPointF tickPos(width()/2, textsOffset+boundTotalHeight+tickPosY);
            ticksPos.append(tickPos.y());

            painter->save();
            painter->translate(tickPos);
            painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
            painter->fillRect(-tickWidth/2, -tickHeight/2, tickWidth, tickHeight, Qt::transparent);

            painter->setFont(m_tickFont);
            pen.setColor(m_tickColor);
            painter->setPen(pen);
            const QRect rectangle = QRect(m_lineWidth/2+m_spacing, -m_tickFont.pixelSize()/2, width()/2-m_lineWidth/2-m_spacing, m_tickFont.pixelSize());
            painter->drawText(rectangle, Qt::AlignVCenter, QString::number(tick, 'F', m_tickDecimal));

            painter->restore();
        }
    }
    setTicksPos(ticksPos);
}
