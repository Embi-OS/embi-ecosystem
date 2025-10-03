#include "timevalidator.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpDigit, ("\\d+"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpTime, ("^$|([01]?[0-9]|2[0-3]):[0-5][0-9]"))

TimeValidator::TimeValidator(QObject* parent) :
    AbstractValidator(parent)
{
    connect(this, &TimeValidator::fromChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &TimeValidator::toChanged, this, &AbstractValidator::queueInvalidate);
}

QTime TimeValidator::textAsTime(const QString& text)
{
    if(text.isEmpty())
        return QTime();
    return QTime::fromString(text, "hh:mm");
}

QString TimeValidator::timeAsText(const QTime& time)
{
    if(time.isNull())
        return QString();
    return time.toString("hh:mm");
}

QValidator::State TimeValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    // Extract current time
    QRegularExpressionMatchIterator matchIterator = regExpDigit->globalMatch(input);

    // If no digit : do nothing
    if(!matchIterator.hasNext())
    {
        input.clear();
        pos = 0;
    }
    else
    {
        QStringList characters;
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            characters.append(match.capturedTexts());
        }

        // Format new input and place cursor

        // Only Hour entered
        if(characters.size()==1)
        {
            const QString hours = characters.at(0);

            // Only first hour char entered
            if (hours.size()==1)
            {
                input = QString("%1_:__").arg(hours);
                pos = 1;
            } // Hour fully entered
            else if (hours.size()==2)
            {
                input = QString("%1:__").arg(hours);
                pos = 2;
            } // Hour entered and first char of minute
            else if (hours.size()==3)
            {
                input = QString("%1%2:%3_").arg(hours[0]).arg(hours[1]).arg(hours[2]);
                pos = 4;
            }
        } // Hour already entered
        else if(characters.size()==2)
        {
            const QString hours = characters.at(0);
            const QString minutes = characters.at(1);

            // Only first minute char entered
            if (minutes.size()==1)
            {
                input = QString("%1:%2_").arg(hours, minutes);
                pos = 4;
            } // Minute fully entered
            else if (minutes.size()==2)
            {
                input = QString("%1:%2").arg(hours, minutes);
                pos = 5;
            } // User trying to put more char in the text field : display only the 2 first minute char
            else
            {
                input = QString("%1:%2%3").arg(hours).arg(minutes[0]).arg(minutes[1]);
                pos = 5;
            }

            // If the user manually moves his cursor, check length of hour and adapt text
            if (hours.size()==1) {
                if (minutes.size()==1)
                {
                    input = QString("0%1:%2_").arg(hours, minutes);
                    pos = 4;
                }
                else if (minutes.size()==2)
                {
                    input = QString("0%1:%2").arg(hours[0]).arg(minutes);
                    pos = 5;
                }
            }
        }
    }

    if(input.contains('_'))
        return QValidator::Intermediate;

    const QRegularExpressionMatch timeMatch = regExpTime->match(input, 0, QRegularExpression::PartialPreferCompleteMatch);

    QValidator::State ret;
    if (timeMatch.hasMatch())
    {
        const QTime time = QTime::fromString(input, "hh:mm");
        if(!m_from.isNull() && time<m_from)
            ret = QValidator::Intermediate;
        else if(!m_to.isNull() && time>m_to)
            ret = QValidator::Intermediate;
        else
            ret = QValidator::Acceptable;
    }
    else if (input.isEmpty() || timeMatch.hasPartialMatch())
        ret = QValidator::Intermediate;
    else
        ret = QValidator::Invalid;

    return ret;
}

void TimeValidator::updateSummary()
{
    QString summary = tr("De %1 à %2").arg(m_from.toString(),m_to.toString());

    setSummary(summary);
}
