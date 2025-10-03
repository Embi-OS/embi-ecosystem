#ifndef PRIMARYHAPPYFIELD_H
#define PRIMARYHAPPYFIELD_H

#include "varhappyfield.h"
class PrimaryHappyField: public VarHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, startValue, StartValue, 1000)

public:
    explicit PrimaryHappyField(QObject* parent=nullptr);

    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;
};

#endif // PRIMARYHAPPYFIELD_H
