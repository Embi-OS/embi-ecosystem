#ifndef ANYOFVALIDATOR_H
#define ANYOFVALIDATOR_H

#include <QModels>

#include "abstractvalidator.h"
class AnyOfValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_CONSTANT_OLP_PROPERTY(AbstractValidator, validators)
    Q_DEFAULT_PROPERTY(validators)

public:
    explicit AnyOfValidator(QObject *parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;
    void fixup(QString &text) const final override;
};

#endif // ANYOFVALIDATOR_H
