#ifndef SOCKETPORTVALIDATOR_H
#define SOCKETPORTVALIDATOR_H

#include "abstractvalidator.h"
class SocketPortValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit SocketPortValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

private:
    QIntValidator* m_intValidator;
};

#endif // SOCKETPORTVALIDATOR_H
