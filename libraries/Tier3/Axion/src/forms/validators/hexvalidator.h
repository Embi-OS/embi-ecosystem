#ifndef HEXVALIDATOR_H
#define HEXVALIDATOR_H

#include "abstractvalidator.h"
class HexValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, top, Top, MAX_INT_VAL)
    Q_WRITABLE_VAR_PROPERTY(int, bottom, Bottom, MIN_INT_VAL)

    Q_WRITABLE_REF_PROPERTY(QList<int>, additionalValues, AdditionalValues, {})

public:
    explicit HexValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

private slots:
    void updateSummary() final override;

private:
    QRegularExpressionValidator* m_regExpValidator;
};

#endif // HEXVALIDATOR_H
