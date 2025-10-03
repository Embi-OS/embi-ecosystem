#ifndef DATEVALIDATOR_H
#define DATEVALIDATOR_H

#include "abstractvalidator.h"
class DateValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QDate, from, From, {})
    Q_WRITABLE_REF_PROPERTY(QDate, to, To, {})

public:
    explicit DateValidator(QObject * parent = nullptr);

    Q_INVOKABLE static QDate textAsDate(const QString& text);
    Q_INVOKABLE static QString dateAsText(const QDate& date);

    QValidator::State validate(QString &, int &) const final override;

private slots:
    void updateSummary() final override;
};

#endif // DATEVALIDATOR_H
