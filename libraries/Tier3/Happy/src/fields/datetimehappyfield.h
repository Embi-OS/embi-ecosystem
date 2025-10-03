#ifndef DATETIMEHAPPYFIELD_H
#define DATETIMEHAPPYFIELD_H

#include "varhappyfield.h"
class DateTimeHappyField: public VarHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, autoNow, AutoNow, false)
    Q_WRITABLE_VAR_PROPERTY(bool, autoNowAdd, AutoNowAdd, false)

public:
    explicit DateTimeHappyField(QObject* parent=nullptr);

    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;
};

#endif // UUIDHAPPYFIELD_H
