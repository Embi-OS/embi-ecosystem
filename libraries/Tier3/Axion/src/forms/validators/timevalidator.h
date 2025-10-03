#ifndef TIMEVALIDATOR_H
#define TIMEVALIDATOR_H

#include "abstractvalidator.h"
class TimeValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QTime, from, From, {})
    Q_WRITABLE_REF_PROPERTY(QTime, to, To, {})

public:
    explicit TimeValidator(QObject * parent = nullptr);

    Q_INVOKABLE static QTime textAsTime(const QString& text);
    Q_INVOKABLE static QString timeAsText(const QTime& time);

    QValidator::State validate(QString &input, int &pos) const final override;

private slots:
    void updateSummary() final override;
};

#endif // TIMEVALIDATOR_H
