#include "allofvalidator.h"
#include "extravalidatorattached.h"

AllOfValidator::AllOfValidator(QObject *parent) :
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

QValidator::State AllOfValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    ExtraValidatorAttached* extra = ExtraValidatorAttached::wrap(this);

    for(AbstractValidator* validator: m_validators)
    {
        QValidator::State state = validator->validate(input, pos);
        if(state!=QValidator::Acceptable) {
            extra->setErrorMessage(ExtraValidatorAttached::wrap(validator)->getErrorMessage());
            return state;
        }
    }

    extra->resetErrorMessage();

    return QValidator::Acceptable;
}

void AllOfValidator::fixup(QString &text) const
{

}
