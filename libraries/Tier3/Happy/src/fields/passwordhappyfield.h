#ifndef PASSWORDHAPPYFIELD_H
#define PASSWORDHAPPYFIELD_H

#include "qpasswordhasher.h"

#include "varhappyfield.h"
class PasswordHappyField: public VarHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_CONSTANT_PTR_PROPERTY(QPasswordHasher, hasher)

public:
    explicit PasswordHappyField(QObject* parent=nullptr);

    bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer) override;

    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;
};

#endif // PASSWORDHAPPYFIELD_H
