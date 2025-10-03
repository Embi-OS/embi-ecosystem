#ifndef STRINGVALIDATOR_H
#define STRINGVALIDATOR_H

#include "abstractvalidator.h"
class StringValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, minSize, MinSize, 0)
    Q_WRITABLE_VAR_PROPERTY(int, maxSize, MaxSize, 0)
    Q_WRITABLE_REF_PROPERTY(QRegularExpression, pattern, Pattern, {})

public:
    explicit StringValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

public slots:
    void invalidate() final override;

protected:
    void updateSummary() final override;

private:
    QRegularExpressionValidator* m_regExpValidator;
};

#endif // STRINGVALIDATOR_H
