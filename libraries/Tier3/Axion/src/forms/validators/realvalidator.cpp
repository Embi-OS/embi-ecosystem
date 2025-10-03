#include "realvalidator.h"

#include <QDefs>
#include <QtMath>

RealValidator::RealValidator(QObject* parent) :
    AbstractValidator(parent),
    m_doubleValidator(new QDoubleValidator(this))
{
    m_doubleValidator->setLocale(locale());
    connect(this, &RealValidator::intervalChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &RealValidator::topChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &RealValidator::bottomChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &RealValidator::decimalsChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &RealValidator::notationChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &RealValidator::autorizeEmptyChanged, this, &AbstractValidator::queueInvalidate);
}

void RealValidator::invalidate()
{
    m_doubleValidator->setLocale(locale());
    m_doubleValidator->setBottom(m_bottom);
    m_doubleValidator->setTop(m_top);
    m_doubleValidator->setDecimals(m_decimals);
    m_doubleValidator->setNotation(m_notation);

    AbstractValidator::invalidate();
}

QValidator::State RealValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    if(qFuzzyIsNull(m_interval))
    {
        if(getAutorizeEmpty() && input=="" && m_bottom<=0 && m_top>=0)
        {
            return QValidator::Acceptable;
        }
    }
    else
    {
        double value = input.toDouble()-m_bottom;

        if(!qFuzzyIsNull(qFuzzyModulo(value,m_interval)))
            return QValidator::Intermediate;

        if(getAutorizeEmpty() && input=="" && m_bottom<=0 && m_top>=0)
            return QValidator::Acceptable;
    }

    return m_doubleValidator->validate(input, pos);
}

void RealValidator::updateSummary()
{
    QString bottom = "-inf";
    if(m_bottom!=-FLOAT_INF)
        bottom = QString::number(m_bottom);

    QString top = "inf";
    if(m_top!=FLOAT_INF)
        top = QString::number(m_top);

    QString summary = tr("%1 à %2").arg(bottom,top);

    setSummary(summary);
}
