#ifndef UUIDHAPPYFIELD_H
#define UUIDHAPPYFIELD_H

#include "varhappyfield.h"
class UuidHappyField: public VarHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, simplify, Simplify, false)

public:
    explicit UuidHappyField(QObject* parent=nullptr);

    bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer) override;

    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;

protected slots:
    void onObjectInserted(const QVariant& primaryValue, const QVariantMap& object);
};

#endif // UUIDHAPPYFIELD_H
