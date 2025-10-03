#ifndef SQLPATCHABLE_H
#define SQLPATCHABLE_H

#include <QDefs>

#include "sql_helpertypes.h"
#include "SqlCore/sqlquerybuilder.h"
#include "syncable/qspatchable.h"

class SqlPatchable : public QObject,
                     public QSPatchable
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, "")

    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})

    Q_WRITABLE_REF_PROPERTY(QList<SqlJoinQuery>, joins, Joins, {})

    Q_WRITABLE_REF_PROPERTY(QVariantList, source, Source, {})

public:
    explicit SqlPatchable(QObject *parent = nullptr);

    QSqlDatabase database() const;
    const QSqlDriver* driver() const;

    const QSqlError& lastError() const;

    bool patch(const QVariantList& destination);

    Q_INVOKABLE bool insert(int index, const QVariant& variant) final override;
    Q_INVOKABLE bool set(int index, const QVariant& variant) final override;
    Q_INVOKABLE bool move(int from, int to, int count = 1) final override;
    Q_INVOKABLE bool remove(int index, int count = 1) final override;

private:
    QStringList m_columns;
    QSqlRecord m_baseRec;
    QSqlIndex m_primaryIndex;
    QSqlError m_lastError;
};

#endif // SQLPATCHABLE_H
