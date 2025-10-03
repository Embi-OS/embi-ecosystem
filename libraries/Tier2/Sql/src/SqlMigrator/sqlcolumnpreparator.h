#ifndef SQLCOLUMNPREPARATOR_H
#define SQLCOLUMNPREPARATOR_H

#include <QDefs>
#include <QModels>
#include "../sql_helpertypes.h"
#include "../SqlCore/sqlcmd.h"

class SqlColumnPreparator : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_VAR_PROPERTY(SqlColumnTypes::Enum, type, Type, SqlColumnTypes::Invalid)
    Q_WRITABLE_VAR_PROPERTY(int, precision, Precision, 0)
    Q_WRITABLE_VAR_PROPERTY(SqlColumnOptions::Flag, options, Options, SqlColumnOptions::None)
    Q_WRITABLE_REF_PROPERTY(QVariant, defaultValue, DefaultValue, {})

public:
    explicit SqlColumnPreparator(QObject* parent = nullptr);
    explicit SqlColumnPreparator(const QString& name, SqlColumnTypes::Enum type, SqlColumnOption options=SqlColumnOptions::None, const QVariant& defaultValue=QVariant(), QObject* parent = nullptr);

    bool isValid() const;
    bool hasDefaultValue() const;

    bool isNullable() const;
    bool isPrimary() const;
    bool isUnique() const;
    bool isAutoIncremented() const;

    QString generateDefinition(const QSqlDriver *driver) const;
    QString generateType(const QSqlDriver *driver) const;
    QString generateOptions(const QSqlDriver *driver) const;
};

#endif // SQLCOLUMNPREPARATOR_H
