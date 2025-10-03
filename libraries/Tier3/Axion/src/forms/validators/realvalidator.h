#ifndef REALVALIDATOR_H
#define REALVALIDATOR_H

#include "abstractvalidator.h"
class RealValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_FUZ_PROPERTY(double, interval, Interval, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, top, Top, DOUBLE_INF)
    Q_WRITABLE_FUZ_PROPERTY(double, bottom, Bottom, -DOUBLE_INF)
    Q_WRITABLE_VAR_PROPERTY(int, decimals, Decimals, -1)
    Q_WRITABLE_VAR_PROPERTY(QDoubleValidator::Notation, notation, Notation, QDoubleValidator::StandardNotation)
    Q_WRITABLE_VAR_PROPERTY(bool, autorizeEmpty, AutorizeEmpty, true)

public:
    explicit RealValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

public slots:
    void invalidate() final override;

protected:
    void updateSummary() final override;

private:
    QDoubleValidator *m_doubleValidator;
};

#endif // REALVALIDATOR_H
