#ifndef INTEGERVALIDATOR_H
#define INTEGERVALIDATOR_H

#include "abstractvalidator.h"
class IntegerValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, top, Top, MAX_INT_VAL)
    Q_WRITABLE_VAR_PROPERTY(int, bottom, Bottom, MIN_INT_VAL)
    Q_WRITABLE_VAR_PROPERTY(int, interval, Interval, 0)
    Q_WRITABLE_VAR_PROPERTY(bool, autorizeEmpty, AutorizeEmpty, true)

public:
    explicit IntegerValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;
    void fixup(QString &input) const final override;

public slots:
    void invalidate() final override;

protected:
    void updateSummary() final override;

private:
    QIntValidator *m_intValidator;
};

#endif // INTEGERVALIDATOR_H
