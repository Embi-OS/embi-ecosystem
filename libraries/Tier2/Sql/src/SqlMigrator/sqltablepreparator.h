#ifndef SQLTABLEPREPARATOR_H
#define SQLTABLEPREPARATOR_H

#include "sqlcolumnpreparator.h"
#include "sqlindexpreparator.h"
#include "sqlprimarycolumnpreparator.h"
class SqlTablePreparator : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_REF_PROPERTY(QVariantList, defaultEntries, DefaultEntries, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, basicEntries, BasicEntries, {})
    Q_WRITABLE_VAR_PROPERTY(SqlMigrationModes::Enum, migrationMode, MigrationMode, SqlMigrationModes::FullAuto)
    Q_CONSTANT_OLP_PROPERTY(SqlColumnPreparator, columns)
    Q_CONSTANT_OLP_PROPERTY(SqlIndexPreparator, indexes)
    Q_DEFAULT_PROPERTY(columns)

public:
    explicit SqlTablePreparator(QObject* parent = nullptr);
    explicit SqlTablePreparator(const QString& name, QObject* parent = nullptr);

    bool isValid() const;
    bool hasColumn(const QString &columnName) const;

    SqlColumnPreparator* getPrimaryColumn() const;

    bool init(const QString& connectionName);

    bool create();
    bool dropIndexes(bool dropUniqueIndex=false);
    bool createIndexes();
    bool drop();
    bool update();
    bool truncate();
    bool updatePrimary();

    bool fillDefaultEntries();
    bool fillBasicEntries();

    bool doAll();
    bool reDoAll();

    QString generateColumnsDefinition() const;

    bool isDefinitionEqualToExistingTable();

private:
    QString m_connectionName = SqlDefaultConnection;
    QSqlDriver *m_driver = nullptr;
};

#endif // SQLTABLEPREPARATOR_H
