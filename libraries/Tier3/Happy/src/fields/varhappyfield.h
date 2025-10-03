#ifndef VARHAPPYFIELD_H
#define VARHAPPYFIELD_H

#include "abstracthappyfield.h"
class VarHappyField: public AbstractHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_COMPOSITION_PROPERTY(SqlColumnPreparator, sqlColumnPreparator, nullptr)

    Q_WRITABLE_VAR_PROPERTY(SqlColumnTypes::Enum, type, Type, SqlColumnTypes::Invalid)
    Q_WRITABLE_VAR_PROPERTY(int, precision, Precision, 0)
    Q_WRITABLE_VAR_PROPERTY(SqlColumnOptions::Flag, options, Options, SqlColumnOptions::None)

public:
    explicit VarHappyField(QObject* parent=nullptr);

    QVariant formatValue(const QVariant& value, bool* ok=nullptr) const override;
    QVariant formatRead(const QVariant& value, bool* ok=nullptr) const override;
    QVariant formatWrite(const QVariant& value, bool* ok=nullptr) const override;
    QVariant read(const QSqlRecord& record, bool* ok=nullptr) const override;
    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;
};

#endif // VARHAPPYFIELD_H
