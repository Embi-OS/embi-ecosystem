#include "ipv4validator.h"

Ipv4Validator::Ipv4Validator(QObject* parent) :
    AbstractValidator(parent),
    m_regExpValidator(new QRegularExpressionValidator(this))
{
    m_regExpValidator->setLocale(locale());
    m_regExpValidator->setRegularExpression(QRegularExpression("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));

    setSummary("IPv4 xxx.xxx.xxx.xxx");
}

QValidator::State Ipv4Validator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    return m_regExpValidator->validate(input, pos);
}
