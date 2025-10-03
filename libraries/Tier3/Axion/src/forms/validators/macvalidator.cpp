#include "macvalidator.h"

MacValidator::MacValidator(QObject* parent) :
    AbstractValidator(parent),
    m_regExpValidator(new QRegularExpressionValidator(this))
{
    m_regExpValidator->setLocale(locale());
    m_regExpValidator->setRegularExpression(QRegularExpression("^((?:[0-9A-Fa-f]{2}(?:[:-]?)){7}(?:[0-9A-Fa-f]{2}))$"));

    connect(this, &MacValidator::bytesChanged, this, &AbstractValidator::queueInvalidate);
}

void MacValidator::invalidate()
{
    m_regExpValidator->setLocale(locale());
    switch(m_bytes) {
    case MacBytes::Data6:
        m_regExpValidator->setRegularExpression(QRegularExpression("^((?:[0-9A-Fa-f]{2}(?:[:-]?)){5}(?:[0-9A-Fa-f]{2}))$"));
        break;
    case MacBytes::Data8:
        m_regExpValidator->setRegularExpression(QRegularExpression("^((?:[0-9A-Fa-f]{2}(?:[:-]?)){7}(?:[0-9A-Fa-f]{2}))$"));
        break;
    }

    AbstractValidator::invalidate();
}


QValidator::State MacValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    return m_regExpValidator->validate(input, pos);
}

void MacValidator::updateSummary()
{
    QString summary = tr("%1 Bytes").arg(m_bytes);

    setSummary(summary);
}
