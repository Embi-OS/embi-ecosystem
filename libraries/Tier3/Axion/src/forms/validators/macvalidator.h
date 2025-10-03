#ifndef MACVALIDATOR_H
#define MACVALIDATOR_H

#include <QDefs>

Q_ENUM_CLASS(MacBytes, MacByte,
             Data6 = 6,
             Data8 = 8,)

#include "abstractvalidator.h"
class MacValidator : public AbstractValidator
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(MacBytes::Enum, bytes, Bytes, MacBytes::Data8)

public:
    explicit MacValidator(QObject * parent = nullptr);

    QValidator::State validate(QString &, int &) const final override;

public slots:
    void invalidate() final override;

protected:
    void updateSummary() final override;

private:
    QRegularExpressionValidator* m_regExpValidator;
};

#endif // MACVALIDATOR_H
