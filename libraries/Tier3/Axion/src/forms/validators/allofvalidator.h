#ifndef ALLOFVALIDATOR_H
#define ALLOFVALIDATOR_H

#include <QModels>

#include "abstractvalidator.h"
class AllOfValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_CONSTANT_OLP_PROPERTY(AbstractValidator, validators)
    Q_DEFAULT_PROPERTY(validators)

public:
    explicit AllOfValidator(QObject *parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;
    void fixup(QString &text) const final override;
};

#endif // ALLOFVALIDATOR_H
