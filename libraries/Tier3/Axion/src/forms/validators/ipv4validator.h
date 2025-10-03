#ifndef IPV4VALIDATOR_H
#define IPV4VALIDATOR_H

#include "abstractvalidator.h"
class Ipv4Validator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit Ipv4Validator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

private:
    QRegularExpressionValidator* m_regExpValidator;
};

#endif // IPV4VALIDATOR_H
