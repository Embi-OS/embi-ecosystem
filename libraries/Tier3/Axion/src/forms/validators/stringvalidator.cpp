#include "stringvalidator.h"

StringValidator::StringValidator(QObject* parent) :
    AbstractValidator(parent),
    m_regExpValidator(new QRegularExpressionValidator(this))
{
    m_regExpValidator->setLocale(locale());

    connect(this, &StringValidator::minSizeChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &StringValidator::maxSizeChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &StringValidator::patternChanged, this, &AbstractValidator::queueInvalidate);
}

void StringValidator::invalidate()
{
    m_regExpValidator->setLocale(locale());
    m_regExpValidator->setRegularExpression(m_pattern);

    AbstractValidator::invalidate();
}

QValidator::State StringValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    if(m_minSize>0 && input.size()<m_minSize)
        return QValidator::Intermediate;

    if(m_maxSize>0 && input.size()>m_maxSize)
        return QValidator::Intermediate;

    if(m_regExpValidator->regularExpression().isValid())
        return m_regExpValidator->validate(input, pos);

    return QValidator::Acceptable;
}

void StringValidator::updateSummary()
{
    QString summary = tr("%1 à %2 charactères").arg(m_minSize).arg(m_maxSize);

    setSummary(summary);
}
