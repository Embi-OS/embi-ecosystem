#include "hexvalidator.h"

HexValidator::HexValidator(QObject* parent) :
    AbstractValidator(parent),
    m_regExpValidator(new QRegularExpressionValidator(this))
{
    m_regExpValidator->setLocale(locale());
    m_regExpValidator->setRegularExpression(QRegularExpression("((0x)|(x))?[a-fA-F0-9]*"));

    connect(this, &HexValidator::topChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &HexValidator::bottomChanged, this, &AbstractValidator::queueInvalidate);
    connect(this, &HexValidator::additionalValuesChanged, this, &AbstractValidator::queueInvalidate);
}

QValidator::State HexValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    QValidator::State regExpResult = m_regExpValidator->validate(input, pos);
    if(regExpResult!=QValidator::Acceptable)
        return regExpResult;

    if(m_top==MAX_INT_VAL && m_bottom==MIN_INT_VAL)
        return QValidator::Acceptable;

    int val = intFromHex(input);

    if(m_additionalValues.contains(val))
        return QValidator::Acceptable;
    if(m_bottom!=MIN_INT_VAL && val < m_bottom)
        return QValidator::Intermediate;
    if(m_top!=MAX_INT_VAL && val > m_top)
        return QValidator::Invalid;

    return QValidator::Acceptable;
}

void HexValidator::updateSummary()
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
