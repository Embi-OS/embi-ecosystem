#include "integervalidator.h"

#include <QDefs>

IntegerValidator::IntegerValidator(QObject* parent) :
    AbstractValidator(parent),
    m_intValidator(new QIntValidator(this))
{
    m_intValidator->setLocale(locale());
    connect(this, &IntegerValidator::topChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &IntegerValidator::bottomChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &IntegerValidator::intervalChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &IntegerValidator::autorizeEmptyChanged, this, &AbstractValidator::queueInvalidate);
}

void IntegerValidator::invalidate()
{
    m_intValidator->setLocale(locale());
    m_intValidator->setBottom(m_bottom);
    m_intValidator->setTop(m_top);

    AbstractValidator::invalidate();
}

QValidator::State IntegerValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    if(m_interval==0)
    {
        if(getAutorizeEmpty() && input=="" && m_bottom<=0 && m_top>=0)
            return QValidator::Acceptable;
    }
    else
    {
        int value = input.toInt()-m_bottom;
        if((value%m_interval)!=0)
            return QValidator::Intermediate;

        if(getAutorizeEmpty() && input=="" && m_bottom<=0 && m_top>=0)
            return QValidator::Acceptable;
    }

    return m_intValidator->validate(input, pos);
}

void IntegerValidator::fixup(QString &input) const
{
    m_intValidator->fixup(input);
}

void IntegerValidator::updateSummary()
{
    QString bottom = "-inf";
    if(m_bottom!=MIN_INT_VAL)
        bottom = QString::number(m_bottom);

    QString top = "inf";
    if(m_top!=MAX_INT_VAL)
        top = QString::number(m_top);

    QString summary = tr("%1 à %2").arg(bottom,top);

    setSummary(summary);
}
