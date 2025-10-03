#include "anyofvalidator.h"

AnyOfValidator::AnyOfValidator(QObject *parent) :
    AbstractValidator(parent),
    m_validators(this)
{
    m_validators.onInserted([this](int index, AbstractValidator* validator) {
        connect(validator, &AbstractValidator::changed, this, &AbstractValidator::queueInvalidate);
    });

    m_validators.onRemoved([this](int index, AbstractValidator* validator) {
        disconnect(validator, nullptr, this, nullptr);
    });
}

QValidator::State AnyOfValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    for(AbstractValidator* validator: m_validators)
    {
        QValidator::State state = validator->validate(input, pos);
        if(state==QValidator::Acceptable)
            return state;
    }

    return QValidator::Intermediate;
}

void AnyOfValidator::fixup(QString &text) const
{

}
