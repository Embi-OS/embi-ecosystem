#ifndef SQLQUERYBUILDER_H
#define SQLQUERYBUILDER_H

#include <QDefs>
#include <QDeferred>
#include <QFuture>

#include "sql_helpertypes.h"

class SqlCondition
{
public:
    explicit SqlCondition(const QString& column);
    explicit SqlCondition(const QString& column, const QVariant& value);
    explicit SqlCondition(const QString& column, SqlFilterOperator op, const QVariant& value);
    virtual ~SqlCondition() = default;

    QSqlDatabase database() const;
    const QSqlDriver* driver() const;
    SqlCondition& connection(const QString& connection);

    SqlCondition& inverted(bool inverted=true);

    SqlCondition& equals(const QVariant& value);
    SqlCondition& notEquals(const QVariant& value);
    SqlCondition& lessThan(const QVariant& value);
    SqlCondition& lessEquals(const QVariant& value);
    SqlCondition& greaterThan(const QVariant& value);
    SqlCondition& greaterEquals(const QVariant& value);
    SqlCondition& range(const QVariant& value);
    SqlCondition& in(const QVariant& value);
    SqlCondition& contains(const QVariant& value);
    SqlCondition& startsWith(const QVariant& value);
    SqlCondition& endsWith(const QVariant& value);
    SqlCondition& regExp(const QVariant& value);

    SqlCondition& operator&&(const QString& other);
    SqlCondition& operator&&(const SqlCondition& other);
    SqlCondition& operator||(const QString& other);
    SqlCondition& operator||(const SqlCondition& other);

    QString build() const;

private:
    QString m_connection;

    QString m_column;
    SqlFilterOperator m_operator;
    QVariant m_value;
    bool m_inverted;

    QStringList m_following;
};

class SqlRawQuery
{
public:
    explicit SqlRawQuery();
    virtual ~SqlRawQuery() = default;

    QSqlDatabase database() const;
    const QSqlDriver* driver() const;
    SqlRawQuery& connection(const QString& connection);

    QString formatValue(const QVariant &value) const;
    QString prepareIdentifier(const QString &identifier, QSqlDriver::IdentifierType type) const;

    SqlRawQuery& sql(const QString& sql);
    SqlRawQuery& trust(bool trust=true);
    SqlRawQuery& forwardOnly(bool forwardOnly=true);
    SqlRawQuery& multiple(bool multiple=true);

    virtual QString build() const;
    QSqlQuery exec(bool* result=nullptr) const;
    QDeferred<QSqlError, QVariant> defer() const;
    QFuture<QSqlError> future() const;
    void async() const;

    static QSqlQuery exec(const QString& sql, bool forwardOnly, bool multiple, const QString& connection, bool* result=nullptr);

protected:
    QString m_connection;
    QString m_sql;

    bool m_trust;
    bool m_forwardOnly;
    mutable bool m_multiple;
};

class SqlSelectQuery;
class SqlJoinQuery
{
    Q_GADGET
    QML_VALUE_TYPE(sqlJoinQuery)
    QML_STRUCTURED_VALUE

    Q_MEMBER_PROPERTY(QString, keyField, {})
    Q_MEMBER_PROPERTY(QString, joinTable, {})
    Q_MEMBER_PROPERTY(QString, joinField, {})
    Q_MEMBER_PROPERTY(QStringList, fields, {})
    Q_MEMBER_PROPERTY(QString, aliasName, {})
    Q_MEMBER_PROPERTY(SqlJoinTypes::Enum, type, SqlJoinTypes::Inner)

public:
    SqlJoinQuery() = default;
    ~SqlJoinQuery() = default;
    Q_INVOKABLE SqlJoinQuery(const SqlJoinQuery &other);

    bool operator==(const SqlJoinQuery&) const = default;
    bool operator!=(const SqlJoinQuery&) const = default;

protected:
    friend SqlSelectQuery;

    QStringList buildFields(const SqlSelectQuery* query) const;
    QString build(const SqlSelectQuery* query) const;
};

class SqlSelectQuery : public SqlRawQuery
{
public:
    explicit SqlSelectQuery();
    virtual ~SqlSelectQuery() = default;

    SqlSelectQuery& select();
    SqlSelectQuery& select(const QString& select);
    SqlSelectQuery& select(const QStringList& fields);

    SqlSelectQuery& aggregate(SqlAggregateType aggregate);
    SqlSelectQuery& distinct(bool distinct=true);

    SqlSelectQuery& from(const QString& table);

    SqlSelectQuery& where(const QString& where);
    SqlSelectQuery& where(const QString& column, const QVariant& value, bool inverted=false);
    SqlSelectQuery& where(const QVariantMap& map, bool inverted=false);
    SqlSelectQuery& where(const SqlCondition& condition);

    SqlSelectQuery& groupBy(const QString& field);
    SqlSelectQuery& groupBy(const QStringList& fields);

    SqlSelectQuery& having(const QString& having);
    SqlSelectQuery& having(const QString& column, const QVariant& value);
    SqlSelectQuery& having(const QVariantMap& map);
    SqlSelectQuery& having(const SqlCondition& condition);

    SqlSelectQuery& orderBy(const QString& sorter);
    SqlSelectQuery& orderBy(const QString& field, Qt::SortOrder order, Qt::CaseSensitivity cs=Qt::CaseInsensitive);
    SqlSelectQuery& orderBy(const QVariantMap& map);

    SqlSelectQuery& join(const QString& joinTable, const QString& keyField, const QString& joinField, const QStringList& fields=QStringList(), SqlJoinType type=SqlJoinTypes::Inner, const QString& aliasName=QString());
    SqlSelectQuery& join(const SqlJoinQuery& join);
    SqlSelectQuery& join(const QList<SqlJoinQuery>& joins);

    SqlSelectQuery& limit(int limit);
    SqlSelectQuery& limit(int limit, int offset);
    SqlSelectQuery& offset(int offset);

    QString build() const final override;

protected:
    friend SqlJoinQuery;

    QString m_select;
    QStringList m_fields;
    QString m_table;
    QStringList m_wheres;
    QStringList m_groups;
    QStringList m_havings;
    QStringList m_sorters;
    QList<SqlJoinQuery> m_joins;
    SqlAggregateType m_aggregate;
    bool m_distinct;
    int m_limit;
    int m_offset;
};

class SqlInsertQuery : public SqlRawQuery
{
public:
    explicit SqlInsertQuery();
    virtual ~SqlInsertQuery() = default;

    SqlInsertQuery& insert(const QVariantMap& data);
    SqlInsertQuery& insert(const QVariantList& data);

    SqlInsertQuery& all(bool all=true);
    SqlInsertQuery& replace(bool replace=true);
    SqlInsertQuery& ignore(bool ignore=true);

    SqlInsertQuery& into(const QString& table);

    QString build() const final override;

protected:
    bool m_all;
    bool m_replace;
    bool m_ignore;
    QString m_table;
    QSet<QString> m_columns;
    QList<QVariantMap> m_values;
};

class SqlUpdateQuery : public SqlRawQuery
{
public:
    explicit SqlUpdateQuery();
    virtual ~SqlUpdateQuery() = default;

    SqlUpdateQuery& update(const QString& table);

    SqlUpdateQuery& set(const QString& column, const QVariant& value);
    SqlUpdateQuery& set(const QVariantMap& values);
    SqlUpdateQuery& operator()(const QString& column, const QVariant& value);

    SqlUpdateQuery& where(const QString& where);
    SqlUpdateQuery& where(const QString& column, const QVariant& value);
    SqlUpdateQuery& where(const QVariantMap& map);
    SqlUpdateQuery& where(const SqlCondition& condition);

    QString build() const final override;

protected:
    QString m_table;
    QVariantMap m_values;
    QStringList m_wheres;
};

class SqlDeleteQuery : public SqlRawQuery
{
public:
    explicit SqlDeleteQuery();
    virtual ~SqlDeleteQuery() = default;

    SqlDeleteQuery& delete_();

    SqlDeleteQuery& from(const QString& table);

    SqlDeleteQuery& where(const QString& where);
    SqlDeleteQuery& where(const QString& column, const QVariant& value);
    SqlDeleteQuery& where(const QVariantMap& map);
    SqlDeleteQuery& where(const SqlCondition& condition);

    QString build() const final override;

protected:
    QString m_table;
    QStringList m_wheres;
};

class SqlMigrationQuery : public SqlRawQuery
{
public:
    explicit SqlMigrationQuery();
    virtual ~SqlMigrationQuery() = default;

    SqlMigrationQuery& alter();
    SqlMigrationQuery& drop();
    SqlMigrationQuery& create();
    SqlMigrationQuery& dropIndex(const QString& name);
    SqlMigrationQuery& createIndex(const QString& name);
    SqlMigrationQuery& truncate();
    SqlMigrationQuery& copy();
    SqlMigrationQuery& vacuum();
    SqlMigrationQuery& analyze();
    SqlMigrationQuery& version();
    SqlMigrationQuery& autoIncrement();
    SqlMigrationQuery& indexList();
    SqlMigrationQuery& tableCreation();

    SqlMigrationQuery& table(const QString& table);
    SqlMigrationQuery& field(const QString& field);
    SqlMigrationQuery& definition(const QString& definition);

    SqlMigrationQuery& rename(const QString& name);
    SqlMigrationQuery& into(const QString& name);
    SqlMigrationQuery& ifExists(bool ifExists=true);
    SqlMigrationQuery& ifNotExists(bool ifNotExists=true);

    QString build() const final override;

protected:
    bool m_alter;
    bool m_drop;
    bool m_create;
    bool m_dropIndex;
    bool m_createIndex;
    bool m_truncate;
    bool m_copy;
    bool m_vacuum;
    bool m_analyze;
    bool m_version;
    bool m_autoIncrement;
    bool m_indexList;
    bool m_tableCreation;

    QString m_table;
    QString m_index;
    QString m_field;
    QString m_rename;
    QString m_into;
    QString m_definition;
    bool m_ifExists;
    bool m_ifNotExists;
};

namespace SqlBuilder
{

QVariant value(QSqlQuery& query, int row, int field);
QVariant value(QSqlQuery& query, int row, const QString& field);
QVariantMap values(QSqlQuery& query, int index);
QVariantList values(QSqlQuery& query);
QVariantList values(QSqlQuery& query, int perPage, int page);

inline SqlRawQuery raw(const QString &sql) {
    return SqlRawQuery().sql(sql);
}

inline SqlSelectQuery select() {
    return SqlSelectQuery().select();
}
inline SqlSelectQuery select(const QString& select) {
    return SqlSelectQuery().select(select);
}
inline SqlSelectQuery select(const QStringList& columns){
    return SqlSelectQuery().select(columns);
}

inline SqlInsertQuery insert(const QVariantMap& data) {
    return SqlInsertQuery().insert(data);
}
inline SqlInsertQuery insert(const QVariantList& data){
    return SqlInsertQuery().insert(data);
}

inline SqlUpdateQuery update(const QString& table){
    return SqlUpdateQuery().update(table);
}

inline SqlDeleteQuery delete_(){
    return SqlDeleteQuery().delete_();
}

inline SqlMigrationQuery drop(){
    return SqlMigrationQuery().drop();
}
inline SqlMigrationQuery alter(){
    return SqlMigrationQuery().alter();
}
inline SqlMigrationQuery create(){
    return SqlMigrationQuery().create();
}
inline SqlMigrationQuery dropIndex(const QString& name){
    return SqlMigrationQuery().dropIndex(name);
}
inline SqlMigrationQuery createIndex(const QString& name){
    return SqlMigrationQuery().createIndex(name);
}
inline SqlMigrationQuery truncate(){
    return SqlMigrationQuery().truncate();
}
inline SqlMigrationQuery copy(){
    return SqlMigrationQuery().copy();
}
inline SqlMigrationQuery vacuum(){
    return SqlMigrationQuery().vacuum();
}
inline SqlMigrationQuery analyze(){
    return SqlMigrationQuery().analyze();
}
inline SqlMigrationQuery version(){
    return SqlMigrationQuery().version();
}
inline SqlMigrationQuery autoIncrement(){
    return SqlMigrationQuery().autoIncrement();
}
inline SqlMigrationQuery indexList(){
    return SqlMigrationQuery().indexList();
}
inline SqlMigrationQuery tableCreation(){
    return SqlMigrationQuery().tableCreation();
}

};

#endif // SQLQUERYBUILDER_H
