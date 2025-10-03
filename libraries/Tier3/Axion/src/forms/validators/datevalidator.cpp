#include "datevalidator.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpDigit, ("\\d+"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpDate, ("^$|([1-9]|0[1-9]|[12][0-9]|3[01])[/](0[1-9]|[1-9]|1[0-2])[/]\\d{4}"))

DateValidator::DateValidator(QObject* parent) :
    AbstractValidator(parent)
{
    connect(this, &DateValidator::fromChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &DateValidator::toChanged, this, &AbstractValidator::queueInvalidate);
}

QDate DateValidator::textAsDate(const QString& input)
{
    if(input.isEmpty())
        return QDate();
    return QDate::fromString(input, "dd/MM/yyyy");
}

QString DateValidator::dateAsText(const QDate& date)
{
    if(date.isNull())
        return QString();
    return date.toString("dd/MM/yyyy");
}

QValidator::State DateValidator::validate(QString &input, int &pos) const
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

        // Only Day entered
        if (characters.size()==1)
        {
            const QString day = characters.at(0);

            // Only first day char entered
            if (day.size()==1)
            {
                input = QString("%1_/__/____").arg(day);
                pos = 1;
            } // Day fully entered
            else if (day.size()==2)
            {
                input = QString("%1/__/____").arg(day);
                pos = 2;
            } // Day entered and first char of month
            else if (day.size()==3)
            {
                input = QString("%1%2/%3_/____").arg(day[0]).arg(day[1]).arg(day[2]);
                pos = 4;
            }
        }
        else if (characters.size()==2)
        {
            const QString day = characters.at(0);
            const QString month = characters.at(1);

            // Only first month char entered
            if (month.size()==1)
            {
                input = QString("%1/%2_/____").arg(day, month);
                pos = 4;
            } // Month fully entered
            else if (month.size()==2)
            {
                input = QString("%1/%2/____").arg(day, month);
                pos = 5;
            } // Month entered and first char of year
            else if (month.size()==3)
            {
                input = QString("%1/%2%3/%4___").arg(day).arg(month[0]).arg(month[1]).arg(month[2]);
                pos = 7;
            } // Month entered and two char of year
            else if (month.size()==4)
            {
                input = QString("%1/%2%3/%4%5__").arg(day).arg(month[0]).arg(month[1]).arg(month[2]).arg(month[3]);
                pos = 8;
            } // Month entered and three char of year
            else if (month.size()==5)
            {
                input = QString("%1/%2%3/%4%5%6_").arg(day).arg(month[0]).arg(month[1]).arg(month[2]).arg(month[3]).arg(month[4]);
                pos = 9;
            } // Else clean input
            else
            {
                input = QString("%1/%2%3/____").arg(day).arg(month[0]).arg(month[1]);
                pos = 6;
            }

            if (day.size()==1)
                input.insert(0, '0');
        }
        else if (characters.size()==3)
        {
            const QString day = characters.at(0);
            const QString month = characters.at(1);
            const QString year = characters.at(2);

            // Only first year char entered
            if (year.size()==1)
            {
                input = QString("%1/%2/%3___").arg(day, month, year);
                pos = 7;
            } // Two year char entered
            else if (year.size()==2)
            {
                input = QString("%1/%2/%3__").arg(day, month, year);
                pos = 8;
            } // Three year char entered
            else if (year.size()==3)
            {
                input = QString("%1/%2/%3_").arg(day, month, year);
                pos = 9;
            } // Year fully entered
            else if (year.size()==4)
            {
                input = QString("%1/%2/%3").arg(day, month, year);
                pos = 10;
            }

            if (day.size()==1)
                input.insert(0, '0');
            if (month.size()==1)
                input.insert(3, '0');
        }
    }


    if(input.contains('_'))
        return QValidator::Intermediate;

    const QRegularExpressionMatch timeMatch = regExpDate->match(input, 0, QRegularExpression::PartialPreferCompleteMatch);

    QValidator::State ret;
    if (timeMatch.hasMatch())
    {
        const QDate date = QDate::fromString(input, "dd/MM/yyyy");
        if(!m_from.isNull() && date<m_from)
            ret = QValidator::Intermediate;
        else if(!m_to.isNull() && date>m_to)
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

void DateValidator::updateSummary()
{
    QString summary = tr("De %1 à %2").arg(m_from.toString(),m_to.toString());

    setSummary(summary);
}
