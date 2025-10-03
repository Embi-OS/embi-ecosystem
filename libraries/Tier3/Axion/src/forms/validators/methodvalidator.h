#ifndef METHODVALIDATOR_H
#define METHODVALIDATOR_H

#include "abstractvalidator.h"
class MethodValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MethodValidator(QObject *parent = nullptr);

    void componentComplete() override;

    QValidator::State validate(QString &, int &) const final override;

protected slots:
    void updateMethod();

private:
    QMetaMethod m_method;
};

#endif // METHODVALIDATOR_H
