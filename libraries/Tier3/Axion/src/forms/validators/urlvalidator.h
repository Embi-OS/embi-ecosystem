#ifndef URLVALIDATOR_H
#define URLVALIDATOR_H

#include "abstractvalidator.h"
class UrlValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QStringList, schemes, Schemes, {})

public:
    explicit UrlValidator(QObject *parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;
    void fixup(QString &text) const final override;

public slots:
    void invalidate() final override;

private:
    QRegularExpressionValidator *m_schemeValidator;
};

#endif // URLVALIDATOR_H
