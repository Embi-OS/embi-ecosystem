#include "sqlquerybuilder.h"
#include "sqlcmd.h"
#include "sqldbpool.h"
#include "sql_log.h"

#include <QtConcurrentRun>

QVariant SqlBuilder::value(QSqlQuery& query, int row, int field)
{
    if(!query.seek(row))
        return QVariant(query.record().field(field).metaType());

    return query.record().value(field);
}

QVariant SqlBuilder::value(QSqlQuery& query, int row, const QString& field)
{
    if(!query.seek(row))
        return QVariant(query.record().field(field).metaType());

    return query.record().value(field);
}

QVariantMap SqlBuilder::values(QSqlQuery& query, int index)
{
    QVariantMap map;
    if(!query.seek(index)) {
        const QSqlRecord rec = query.record();
        for(int i = 0; i < rec.count(); ++i)
            map.insert(rec.fieldName(i), QVariant(rec.field(i).metaType()));
        return map;
    }

    const QSqlRecord rec = query.record();
    for(int i = 0; i < rec.count(); ++i)
        map.insert(rec.fieldName(i), rec.value(i));

    return map;
}

QVariantList SqlBuilder::values(QSqlQuery& query)
{
    return values(query, 0, 0);
}

QVariantList SqlBuilder::values(QSqlQuery& query, int perPage, int page)
{
    const long long size = query.size();

    QVariantList list;
    list.reserve(size);

    if(page>0)
    {
        int index = (page-1)*perPage;
        bool exists = query.seek(index);
        while(exists && (perPage==0 || list.size()<perPage))
        {
            const QSqlRecord rec = query.record();
            QVariantMap resultRow;
            for(int i = 0; i < rec.count(); ++i)
                resultRow.insert(rec.fieldName(i), rec.value(i));
            list.append(resultRow);
            exists = query.next();
        }
    }
    else
    {
        bool exists = query.seek(0);
        while(exists)
        {
            const QSqlRecord rec = query.record();
            QVariantMap resultRow;
            for(int i = 0; i < rec.count(); ++i)
                resultRow.insert(rec.fieldName(i), rec.value(i));
            list.append(resultRow);
            exists = query.next();
        }
    }

    return list;
}

//──────────────────────────────────────────────────────────────────────
// SqlCondition
//──────────────────────────────────────────────────────────────────────

SqlCondition::SqlCondition(const QString& column):
    SqlCondition(column, QVariant())
{

}

SqlCondition::SqlCondition(const QString& column, const QVariant& value):
    SqlCondition(column, SqlFilterOperators::Equals, value)
{

}

SqlCondition::SqlCondition(const QString& column, SqlFilterOperator op, const QVariant& value):
    m_column(column),
    m_operator(op),
    m_value(value),
    m_inverted(false)
{

}

QSqlDatabase SqlCondition::database() const
{
    return SqlDbPool::database(m_connection);
}

const QSqlDriver* SqlCondition::driver() const
{
    return database().driver();
}

SqlCondition& SqlCondition::connection(const QString& connection)
{
    m_connection = connection;
    return *this;
}

SqlCondition& SqlCondition::inverted(bool inverted)
{
    m_inverted = inverted;
    return *this;
}

SqlCondition& SqlCondition::equals(const QVariant& value)
{
    m_operator = SqlFilterOperators::Equals;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::notEquals(const QVariant& value)
{
    m_operator = SqlFilterOperators::NotEquals;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::lessThan(const QVariant& value)
{
    m_operator = SqlFilterOperators::LessThan;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::lessEquals(const QVariant& value)
{
    m_operator = SqlFilterOperators::LessEquals;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::greaterThan(const QVariant& value)
{
    m_operator = SqlFilterOperators::GreaterThan;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::greaterEquals(const QVariant& value)
{
    m_operator = SqlFilterOperators::GreaterEquals;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::range(const QVariant& value)
{
    m_operator = SqlFilterOperators::Range;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::in(const QVariant& value)
{
    m_operator = SqlFilterOperators::In;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::contains(const QVariant& value)
{
    m_operator = SqlFilterOperators::Contains;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::startsWith(const QVariant& value)
{
    m_operator = SqlFilterOperators::StartsWith;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::endsWith(const QVariant& value)
{
    m_operator = SqlFilterOperators::EndsWith;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::regExp(const QVariant& value)
{
    m_operator = SqlFilterOperators::RegExp;
    m_value = value;
    return *this;
}

SqlCondition& SqlCondition::operator&&(const QString& condition)
{
    QString sql;
    sql = Sql::AND();
    sql = Sql::concat(sql, QString("(%1)").arg(condition));
    m_following.append(sql);
    return *this;
}

SqlCondition& SqlCondition::operator&&(const SqlCondition& condition)
{
    return *this && (condition.build());
}

SqlCondition& SqlCondition::operator||(const QString& condition)
{
    QString sql;
    sql = Sql::OR();
    sql = Sql::concat(sql, QString("(%1)").arg(condition));
    m_following.append(sql);
    return *this;
}

SqlCondition& SqlCondition::operator||(const SqlCondition& condition)
{
    return *this || (condition.build());
}

QString SqlCondition::build() const
{
    QString sql;
    sql.reserve(2048);

    const QString column = Sql::prepareIdentifier(m_column, QSqlDriver::FieldName, driver());
    const QString value = Sql::formatValue(m_value, driver());

    switch (m_operator) {
    case SqlFilterOperators::Equals:
        if (value.isNull())
            sql = QString("%1 IS NULL").arg(column);
        else if(value.startsWith('(') && value.endsWith(')'))
            sql = QString("%1 IN %2").arg(column, value);
        else if(!value.isEmpty())
            sql = QString("%1 = %2").arg(column, value);
        break;
    case SqlFilterOperators::NotEquals:
        if (value.isNull())
            sql = QString("%1 IS NOT NULL").arg(column);
        else if(value.startsWith('(') && value.endsWith(')'))
            sql = QString("%1 NOT IN %2").arg(column, value);
        else if(!value.isEmpty())
            sql = QString("%1 != %2").arg(column, value);
        break;
    case SqlFilterOperators::LessThan:
        sql = QString("%1 < %2").arg(column, value);
        break;
    case SqlFilterOperators::LessEquals:
        sql = QString("%1 <= %2").arg(column, value);
        break;
    case SqlFilterOperators::GreaterThan:
        sql = QString("%1 > %2").arg(column, value);
        break;
    case SqlFilterOperators::GreaterEquals:
        sql = QString("%1 >= %2").arg(column, value);
        break;
    case SqlFilterOperators::Range: {
        const QVariantList values = m_value.toList();
        if(values.size()<2)
            break;
        const QString valueLower = Sql::formatValue(values.at(0), driver());
        const QString valueUpper = Sql::formatValue(values.at(1), driver());
        sql = QString("%1 BETWEEN %2 AND %3").arg(column, valueLower, valueUpper);
    }
        break;
    case SqlFilterOperators::In:
        if (value.isNull())
            sql = QString("%1 IS NULL").arg(column);
        else if(value.startsWith('(') && value.endsWith(')'))
            sql = QString("%1 IN %2").arg(column, value);
        else if(!value.isEmpty())
            sql = QString("%1 = %2").arg(column, value);
        break;
    case SqlFilterOperators::IContains:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%%1%").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::IStartsWith:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%1%").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::IEndsWith:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%%1").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::Contains:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%%1%").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::StartsWith:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%1%").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::EndsWith:
        sql = QString("%1 LIKE %2").arg(column, Sql::formatValue(QString("%%1").arg(m_value.toString()), driver()));
        break;
    case SqlFilterOperators::RegExp:
        sql = QString("%1 REGEXP %2").arg(column, value);
        break;
    }

    if(m_inverted)
        sql = Sql::inv(sql);

    sql = Sql::concat(sql, m_following.join(' '));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlRawQuery
//──────────────────────────────────────────────────────────────────────

SqlRawQuery::SqlRawQuery():
    m_connection(SqlDefaultConnection),
    m_trust(false),
    m_forwardOnly(false),
    m_multiple(false)
{

}

QString SqlRawQuery::formatValue(const QVariant &value) const
{
    return Sql::formatValue(value, driver(), false);
}

QString SqlRawQuery::prepareIdentifier(const QString &identifier, QSqlDriver::IdentifierType type) const
{
    return Sql::prepareIdentifier(identifier, type, driver());
}

SqlRawQuery& SqlRawQuery::sql(const QString& sql)
{
    m_sql = sql;
    return *this;
}

SqlRawQuery& SqlRawQuery::trust(bool trust)
{
    m_trust = trust;
    return *this;
}

SqlRawQuery& SqlRawQuery::forwardOnly(bool forwardOnly)
{
    m_forwardOnly = forwardOnly;
    return *this;
}

SqlRawQuery& SqlRawQuery::multiple(bool multiple)
{
    m_multiple = multiple;
    return *this;
}

QSqlDatabase SqlRawQuery::database() const
{
    return SqlDbPool::database(m_connection);
}

const QSqlDriver* SqlRawQuery::driver() const
{
    return database().driver();
}

SqlRawQuery& SqlRawQuery::connection(const QString& connection)
{
    m_connection = connection;
    return *this;
}

QString SqlRawQuery::build() const
{
    return m_sql;
}

QSqlQuery SqlRawQuery::exec(bool* result) const
{
    const QString sql = build();

    return exec(sql, m_forwardOnly, m_multiple, m_connection, result);
}

QDeferred<QSqlError, QVariant> SqlRawQuery::defer() const
{
    QDeferred<QSqlError, QVariant> defer;

    const QString sql = build();

    QFuture<QSqlError> future = QtConcurrent::run([defer](const QString& sql, bool forwardOnly, bool multiple, const QString& connection) mutable {
        bool result=false;
        QSqlQuery query = SqlRawQuery::exec(sql, forwardOnly, multiple, connection, &result);
        defer.end(!query.lastError().isValid(), query.lastError(), query.lastInsertId());
        return query.lastError();
    }, sql, m_forwardOnly, m_multiple, m_connection);

    return defer;
}

QFuture<QSqlError> SqlRawQuery::future() const
{
    const QString sql = build();

    QFuture<QSqlError> future = QtConcurrent::run([](const QString& sql, bool forwardOnly, bool multiple, const QString& connection) {
        bool result=false;
        QSqlQuery query = SqlRawQuery::exec(sql, forwardOnly, multiple, connection, &result);
        return query.lastError();
    }, sql, m_forwardOnly, m_multiple, m_connection);

    return future;
}

void SqlRawQuery::async() const
{
    future();
}

QSqlQuery SqlRawQuery::exec(const QString& sql, bool forwardOnly, bool multiple, const QString& connection, bool* result)
{
    QElapsedTimer timer;
    timer.start();

    QSqlDatabase db = SqlDbPool::database(connection);

    if(!db.isValid())
    {
        SQLLOG_WARNING()<<"invalid connection"<<db.connectionName();
        return QSqlQuery(db);
    }

    if(!db.isOpen())
    {
        SQLLOG_WARNING()<<"DB is not open";
        return QSqlQuery(db);
    }

    if(sql.isEmpty())
    {
        SQLLOG_DEBUG()<<"Empty query";
        return QSqlQuery(db);
    }

    bool queryResult = false;
    QSqlQuery query(db);
    query.setForwardOnly(forwardOnly);
    if(multiple) {
        const QStringList sqls = sql.split(';', Qt::SkipEmptyParts);
        for(const QString& sql: sqls) {
            SQLLOG_TRACE()<<"Complete query-string looks like:";
            SQLLOG_TRACE()<<sql;
            QSqlQuery query(db);
            query.setForwardOnly(forwardOnly);
            queryResult = query.exec(sql.simplified());
            if(!queryResult)
                break;
        }
    }
    else {
        SQLLOG_TRACE()<<"Complete query-string looks like:";
        SQLLOG_TRACE()<<sql;
        queryResult = query.exec(sql);
    }

    SQLLOG_TRACE()<<"Query in"<<db.connectionName()<<"took"<<timer.nsecsElapsed()/1000000.0<<"ms";
    // qNotice().noquote()<<sql;
    // qNotice()<<"Query in"<<db.connectionName()<<"took"<<timer.nsecsElapsed()/1000000.0<<"ms";

    if(result!=nullptr)
        *result=queryResult;

    if(!queryResult)
    {
        const QSqlError error = query.lastError();
        SQLLOG_WARNING()<<"Error while executing query!";
        SQLLOG_WARNING()<<sql;
        SQLLOG_WARNING()<<error;
    }

    return query;
}

//──────────────────────────────────────────────────────────────────────
// SqlJoinQuery
//──────────────────────────────────────────────────────────────────────

SqlJoinQuery::SqlJoinQuery(const SqlJoinQuery &other):
    keyField(other.keyField),
    joinTable(other.joinTable),
    joinField(other.joinField),
    fields(other.fields),
    aliasName(other.aliasName),
    type(other.type)
{

}

QStringList SqlJoinQuery::buildFields(const SqlSelectQuery* query) const
{
    QStringList sqlFields;

    const QString sqlTable = query->prepareIdentifier(joinTable, QSqlDriver::TableName);

    const QSqlRecord record = query->database().record(joinTable);
    if (record.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table:"<<joinTable;
    }

    for (int i = 0; i < record.count(); ++i) {
        const QString field = record.fieldName(i);
        if(fields.isEmpty() || fields.contains(field)) {
            const QString sqlColumn = query->prepareIdentifier(field, QSqlDriver::FieldName);
            const QString sqlField = sqlTable % '.' % sqlColumn;
            const QString sqlPrefix = aliasName.isEmpty() ? keyField : aliasName;
            const QString sqlAlias = query->prepareIdentifier(sqlPrefix % '_' % field, QSqlDriver::FieldName);
            sqlFields.append(Sql::as(sqlField, sqlAlias));
        }
    }

    return sqlFields;
}

QString SqlJoinQuery::build(const SqlSelectQuery* query) const
{
    const QString sqlJoinTable = query->prepareIdentifier(joinTable, QSqlDriver::TableName);
    const QString sqlTable = query->prepareIdentifier(query->m_table, QSqlDriver::TableName);

    QString sql;
    sql.reserve(2048);

    switch (type) {
    case SqlJoinTypes::Inner:
        sql = "INNER JOIN";
        break;
    case SqlJoinTypes::Left:
        sql = "LEFT JOIN";
        break;
    case SqlJoinTypes::Right:
        sql = "RIGHT JOIN";
        break;
    case SqlJoinTypes::Cross:
        sql = "CROSS JOIN";
        break;
    }

    const QString sqlKeyColumn = query->prepareIdentifier(keyField, QSqlDriver::FieldName);
    const QString sqlKey = sqlTable % '.' % sqlKeyColumn;

    const QString sqlJoinColumn = query->prepareIdentifier(joinField, QSqlDriver::FieldName);
    const QString sqlJoin = sqlJoinTable % '.' % sqlJoinColumn;

    sql = Sql::concat(sql, sqlJoinTable);
    sql = Sql::concat(sql, Sql::on(QString("%1=%2").arg(sqlKey, sqlJoin)));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlSelectQuery
//──────────────────────────────────────────────────────────────────────

SqlSelectQuery::SqlSelectQuery():
    SqlRawQuery(),
    m_aggregate(SqlAggregateTypes::None),
    m_distinct(false),
    m_limit(0),
    m_offset(0)
{

}

SqlSelectQuery& SqlSelectQuery::select()
{
    m_select.clear();
    m_fields.clear();
    return *this;
}

SqlSelectQuery& SqlSelectQuery::select(const QString& select)
{
    m_select = select;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::select(const QStringList& fields)
{
    m_fields = fields;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::aggregate(SqlAggregateType aggregate)
{
    m_aggregate = aggregate;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::distinct(bool distinct)
{
    m_distinct = distinct;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::from(const QString& table)
{
    m_table = table;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::where(const QString& where)
{
    m_wheres.append(where);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::where(const QString& column, const QVariant& value, bool inverted)
{
    QString field = column;
    SqlFilterOperator sqlOp = SqlFilterOperators::Equals;
    if(column.contains("__")) {
        QStringList params = column.split("__");
        if(Sql::isFilterNameValid(params.last())) {
            const QString op = params.takeLast();
            sqlOp = Sql::filterOperator(op);
            field = params.join("__");
        }
    }

    if(inverted)
        return where(SqlCondition(field, sqlOp, value).inverted().connection(m_connection));
    return where(SqlCondition(field, sqlOp, value).connection(m_connection));
}

SqlSelectQuery& SqlSelectQuery::where(const QVariantMap& map, bool inverted)
{
    for(auto [key, value]: map.asKeyValueRange())
        where(key, value, inverted);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::where(const SqlCondition& condition)
{
    return where(condition.build());
}

SqlSelectQuery& SqlSelectQuery::groupBy(const QString& field)
{
    m_groups.append(this->prepareIdentifier(field, QSqlDriver::FieldName));
    return *this;
}

SqlSelectQuery& SqlSelectQuery::groupBy(const QStringList& fields)
{
    for(const QString& field: fields)
        groupBy(field);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::having(const QString& having)
{
    m_havings.append(having);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::having(const QString& column, const QVariant& value)
{
    return having(SqlCondition(column).equals(value).connection(m_connection));
}

SqlSelectQuery& SqlSelectQuery::having(const QVariantMap& map)
{
    for(auto [key, value]: map.asKeyValueRange())
        having(key, value);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::having(const SqlCondition& condition)
{
    having(condition.build());
    return *this;
}

SqlSelectQuery& SqlSelectQuery::orderBy(const QString& sorter)
{
    m_sorters.append(sorter);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::orderBy(const QString& field, Qt::SortOrder order, Qt::CaseSensitivity cs)
{
    QString sorter;
    sorter = this->prepareIdentifier(field, QSqlDriver::FieldName);
    if(cs==Qt::CaseInsensitive) {
        if(driver()->dbmsType()==QSqlDriver::SQLite && sorter.contains("JSON_EXTRACT"))
            sorter = Sql::collateNocase(sorter);
    }
    sorter = order == Qt::AscendingOrder ? Sql::asc(sorter) : Sql::desc(sorter);
    return orderBy(sorter);
}

SqlSelectQuery& SqlSelectQuery::orderBy(const QVariantMap& map)
{
    for(auto [key, value]: map.asKeyValueRange())
        orderBy(key, (Qt::SortOrder)value.toInt());
    return *this;
}

SqlSelectQuery& SqlSelectQuery::join(const QString& joinTable, const QString& keyField, const QString& joinField, const QStringList& fields, SqlJoinType type, const QString& aliasName)
{
    SqlJoinQuery join;

    join.joinTable = joinTable;
    join.keyField = keyField;
    join.joinField = joinField;
    join.fields = fields;
    join.type = type;
    join.aliasName = aliasName;

    m_joins.append(join);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::join(const SqlJoinQuery& join)
{
    m_joins.append(join);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::join(const QList<SqlJoinQuery>& joins)
{
    m_joins.append(joins);
    return *this;
}

SqlSelectQuery& SqlSelectQuery::limit(int limit)
{
    m_limit = limit;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::limit(int limit, int offset)
{
    m_limit = limit;
    m_offset = offset;
    return *this;
}

SqlSelectQuery& SqlSelectQuery::offset(int offset)
{
    m_offset = offset;
    return *this;
}

QString SqlSelectQuery::build() const
{
    QString sql;
    sql.reserve(2048);

    sql = "SELECT";

    const QString table = this->prepareIdentifier(m_table, QSqlDriver::TableName);

    QStringList joiners;
    for (const SqlJoinQuery& join: m_joins)
        joiners << join.build(this);

    if(m_distinct) {
        sql = Sql::distinct(sql);
    }

    QString select = m_select;
    if(select.isEmpty()) {
        QStringList columns;

        if(!m_trust)
        {
            const QSqlRecord record = database().record(m_table);
            if (record.isEmpty()) {
                SQLLOG_WARNING()<<"Unable to find table:"<<m_table;
                return QString();
            }

            for (int i = 0; i < record.count(); ++i) {
                const QString field = record.fieldName(i);
                if(m_fields.isEmpty() || m_fields.contains(field)) {
                    const QString sqlColumn = this->prepareIdentifier(field, QSqlDriver::FieldName);
                    columns.append(sqlColumn);
                }
            }
        }
        else
        {
            for (const QString& field: m_fields) {
                const QString sqlColumn = this->prepareIdentifier(field, QSqlDriver::FieldName);
                columns.append(sqlColumn);
            }
        }

        for (const SqlJoinQuery& join: m_joins) {
            columns.append(join.buildFields(this));
        }

        select = columns.join(',');
    }

    switch (m_aggregate) {
    case SqlAggregateTypes::Count:
        sql = Sql::concat(sql, Sql::count(select));
        break;
    case SqlAggregateTypes::Avg:
        sql = Sql::concat(sql, Sql::avg(select));
        break;
    case SqlAggregateTypes::Sum:
        sql = Sql::concat(sql, Sql::sum(select));
        break;
    case SqlAggregateTypes::Min:
        sql = Sql::concat(sql, Sql::min(select));
        break;
    case SqlAggregateTypes::Max:
        sql = Sql::concat(sql, Sql::max(select));
        break;
    default:
        sql = Sql::concat(sql, select);
        break;
    }

    sql = Sql::concat(sql, Sql::from(table));
    sql = Sql::concat(sql, joiners.join(' '));
    sql = Sql::concat(sql, Sql::where(m_wheres.join(" AND ")));
    sql = Sql::concat(sql, Sql::groupBy(m_groups.join(',')));
    sql = Sql::concat(sql, Sql::having(m_havings.join(" AND ")));
    sql = Sql::concat(sql, Sql::orderBy(m_sorters.join(',')));
    sql = Sql::concat(sql, Sql::limit(m_limit));
    sql = Sql::concat(sql, Sql::offset(m_offset));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlInsertQuery
//──────────────────────────────────────────────────────────────────────

SqlInsertQuery::SqlInsertQuery():
    SqlRawQuery(),
    m_all(false),
    m_replace(false),
    m_ignore(false)
{

}

SqlInsertQuery& SqlInsertQuery::insert(const QVariantMap& data)
{
    const QStringList columns = data.keys();
    m_columns.unite(QSet<QString>(columns.begin(), columns.end()));
    m_values.append(data);
    return *this;
}

SqlInsertQuery& SqlInsertQuery::insert(const QVariantList& data)
{
    for(const QVariant& data: data)
        insert(data.toMap());
    return *this;
}

SqlInsertQuery& SqlInsertQuery::all(bool all)
{
    m_all = all;
    return *this;
}

SqlInsertQuery& SqlInsertQuery::replace(bool replace)
{
    m_replace = replace;
    return *this;
}

SqlInsertQuery& SqlInsertQuery::ignore(bool ignore)
{
    m_ignore = ignore;
    return *this;
}

SqlInsertQuery& SqlInsertQuery::into(const QString& table)
{
    m_table = table;
    return *this;
}

QString SqlInsertQuery::build() const
{
    if (m_values.isEmpty()) {
        SQLLOG_DEBUG()<<"Nothing to insert into:"<<m_table;
        return QString();
    }

    const QSqlRecord record = database().record(m_table);
    if (record.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table:"<<m_table;
        return QString();
    }

    QStringList columns;
    for(int i=0; i<record.count(); i++) {
        const QString column = record.fieldName(i);
        if(m_columns.contains(column) || m_all)
            columns.append(this->prepareIdentifier(column, QSqlDriver::FieldName));
    }
    QStringList values;
    for(const QVariantMap& map: m_values) {

        QStringList mapValues;
        for(int i=0; i<record.count(); i++) {
            const QString column = record.fieldName(i);
            if(m_columns.contains(column) || m_all)
                mapValues.append(this->formatValue(map.value(record.fieldName(i))));
        }
        values.append(QString("(%1)").arg(mapValues.join(',')));
    }

    QString sql;
    sql.reserve(2048);

    if (m_replace) {
        if(driver()->dbmsType()==QSqlDriver::SQLite)
            sql.append("INSERT OR REPLACE");
        else
            sql.append("REPLACE");
    } else if (m_ignore) {
        if(driver()->dbmsType()==QSqlDriver::SQLite)
            sql.append("INSERT OR IGNORE");
        else
            sql.append("INSERT IGNORE");
    } else {
        sql.append("INSERT");
    }

    sql = Sql::concat(sql, Sql::into(this->prepareIdentifier(m_table, QSqlDriver::TableName)));
    sql = Sql::concat(sql, QString("(%1)").arg(columns.join(',')));
    sql = Sql::concat(sql, Sql::values(values.join(',')));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlUpdateQuery
//──────────────────────────────────────────────────────────────────────

SqlUpdateQuery::SqlUpdateQuery():
    SqlRawQuery()
{

}

SqlUpdateQuery& SqlUpdateQuery::update(const QString& table)
{
    m_table = table;
    return *this;
}

SqlUpdateQuery& SqlUpdateQuery::set(const QString& column, const QVariant& value)
{
    m_values.insert(column, value);
    return *this;
}

SqlUpdateQuery& SqlUpdateQuery::set(const QVariantMap& values)
{
    for (auto [key, value] : values.asKeyValueRange())
        set(key, value);
    return *this;
}

SqlUpdateQuery& SqlUpdateQuery::operator()(const QString& column, const QVariant& value)
{
    return set(column, value);
}

SqlUpdateQuery& SqlUpdateQuery::where(const QString& where)
{
    m_wheres.append(where);
    return *this;
}

SqlUpdateQuery& SqlUpdateQuery::where(const QString& column, const QVariant& value)
{
    return where(SqlCondition(column).equals(value).connection(m_connection));
}

SqlUpdateQuery& SqlUpdateQuery::where(const QVariantMap& map)
{
    for(auto [key, value]: map.asKeyValueRange())
        where(key, value);
    return *this;
}

SqlUpdateQuery& SqlUpdateQuery::where(const SqlCondition& condition)
{
    return where(condition.build());
}

QString SqlUpdateQuery::build() const
{
    if (m_values.isEmpty()) {
        SQLLOG_DEBUG()<<"Nothing to update into:"<<m_table;
        return QString();
    }

    const QSqlRecord record = database().record(m_table);
    if (record.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table:"<<m_table;
        return QString();
    }

    QStringList updates;
    for (int i = 0; i < record.count(); ++i) {
        const QString column = record.fieldName(i);
        if(m_values.contains(column))
            updates.append(QString("%1=%2").arg(this->prepareIdentifier(column, QSqlDriver::FieldName), this->formatValue(m_values.value(column))));
    }

    if(updates.isEmpty()) {
        SQLLOG_DEBUG()<<"Nothing to update";
        return QString();
    }

    QString sql;
    sql.reserve(2048);

    sql = "UPDATE";
    sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
    sql = Sql::concat(sql, Sql::set());
    sql = Sql::concat(sql, updates.join(','));
    sql = Sql::concat(sql, Sql::where(m_wheres.join(" AND ")));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlDeleteQuery
//──────────────────────────────────────────────────────────────────────

SqlDeleteQuery::SqlDeleteQuery():
    SqlRawQuery()
{

}

SqlDeleteQuery& SqlDeleteQuery::delete_()
{
    return *this;
}

SqlDeleteQuery& SqlDeleteQuery::from(const QString& table)
{
    m_table = table;
    return *this;
}

SqlDeleteQuery& SqlDeleteQuery::where(const QString& where)
{
    m_wheres.append(where);
    return *this;
}

SqlDeleteQuery& SqlDeleteQuery::where(const QString& column, const QVariant& value)
{
    return where(SqlCondition(column).equals(value).connection(m_connection));
}

SqlDeleteQuery& SqlDeleteQuery::where(const QVariantMap& map)
{
    for(auto [key, value]: map.asKeyValueRange())
        where(key, value);
    return *this;
}

SqlDeleteQuery& SqlDeleteQuery::where(const SqlCondition& condition)
{
    return where(condition.build());
}

QString SqlDeleteQuery::build() const
{
    if(!m_trust)
    {
        const QSqlRecord record = database().record(m_table);
        if (record.isEmpty()) {
            SQLLOG_WARNING()<<"Unable to find table:"<<m_table;
            return QString();
        }
    }

    if (m_wheres.isEmpty()) {
        SQLLOG_DEBUG()<<"Nothing to delete into:"<<m_table;
        return QString();
    }

    QString sql;
    sql.reserve(2048);

    sql = "DELETE";
    sql = Sql::concat(sql, Sql::from(this->prepareIdentifier(m_table, QSqlDriver::TableName)));
    sql = Sql::concat(sql, Sql::where(m_wheres.join(" AND ")));

    return sql;
}

//──────────────────────────────────────────────────────────────────────
// SqlMigrationQuery
//──────────────────────────────────────────────────────────────────────

SqlMigrationQuery::SqlMigrationQuery():
    SqlRawQuery(),
    m_alter(false),
    m_drop(false),
    m_create(false),
    m_dropIndex(false),
    m_createIndex(false),
    m_truncate(false),
    m_copy(false),
    m_vacuum(false),
    m_analyze(false),
    m_version(false),
    m_autoIncrement(false),
    m_indexList(false),
    m_tableCreation(false),
    m_ifExists(false),
    m_ifNotExists(false)
{

}

SqlMigrationQuery& SqlMigrationQuery::alter()
{
    m_alter = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::drop()
{
    m_drop = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::create()
{
    m_create = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::dropIndex(const QString& name)
{
    m_dropIndex = true;
    m_index = name;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::createIndex(const QString& name)
{
    m_createIndex = true;
    m_index = name;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::truncate()
{
    m_truncate = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::copy()
{
    m_copy = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::vacuum()
{
    m_vacuum = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::analyze()
{
    m_analyze = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::version()
{
    m_version = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::autoIncrement()
{
    m_autoIncrement = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::indexList()
{
    m_indexList = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::tableCreation()
{
    m_tableCreation = true;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::table(const QString& table)
{
    m_table = table;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::field(const QString& field)
{
    m_field = field;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::rename(const QString& name)
{
    m_rename = name;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::into(const QString& name)
{
    m_into = name;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::definition(const QString& definition)
{
    m_definition = definition;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::ifExists(bool ifExists)
{
    m_ifExists = ifExists;
    return *this;
}

SqlMigrationQuery& SqlMigrationQuery::ifNotExists(bool ifNotExists)
{
    m_ifNotExists = ifNotExists;
    return *this;
}

QString SqlMigrationQuery::build() const
{
    QString sql;
    sql.reserve(2048);

    if(m_drop && !m_table.isEmpty()) {
        sql = "DROP TABLE";
        if(m_ifExists)
            sql = Sql::concat(sql, "IF EXISTS");
        sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
    }
    else if(m_alter && !m_table.isEmpty() && !m_rename.isEmpty()) {
        sql = "ALTER TABLE";
        sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        sql = Sql::concat(sql, "RENAME TO");
        sql = Sql::concat(sql, this->prepareIdentifier(m_rename, QSqlDriver::TableName));
    }
    else if(m_create && !m_table.isEmpty() && !m_definition.isEmpty()) {
        sql = "CREATE TABLE";
        if(m_ifNotExists)
            sql = Sql::concat(sql, "IF NOT EXISTS");
        sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        sql = Sql::concat(sql, QString("(%1)").arg(m_definition));
    }
    else if(m_dropIndex && !m_index.isEmpty() && !m_table.isEmpty()) {
        sql = "DROP INDEX";
        if(m_ifExists)
            sql = Sql::concat(sql, "IF EXISTS");
        sql = Sql::concat(sql, this->prepareIdentifier(m_index, QSqlDriver::FieldName));

        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = Sql::concat(sql, Sql::on());
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
    }
    else if(m_createIndex && !m_index.isEmpty() && !m_table.isEmpty() && !m_field.isEmpty()) {
        sql = "CREATE INDEX";
        if(m_ifNotExists)
            sql = Sql::concat(sql, "IF NOT EXISTS");
        sql = Sql::concat(sql, this->prepareIdentifier(m_index, QSqlDriver::FieldName));
        sql = Sql::concat(sql, Sql::on());
        sql = Sql::concat(sql, QString("%1(%2)").arg(this->prepareIdentifier(m_table, QSqlDriver::TableName), this->prepareIdentifier(m_field, QSqlDriver::FieldName)));
    }
    else if(m_truncate && !m_table.isEmpty()) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = "TRUNCATE TABLE";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
        else if(dbmsType==QSqlDriver::SQLite) {
            sql = "DELETE FROM";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
    }
    else if(m_copy && !m_table.isEmpty() && !m_into.isEmpty()) {
        QStringList oldColumns;
        const QSqlRecord oldRecord = driver()->record(m_table);
        for(int i = 0; i < oldRecord.count(); i++)
            oldColumns.append(oldRecord.field(i).name());

        QStringList newColumns;
        const QSqlRecord newRecord = driver()->record(m_into);
        for(int i = 0; i < newRecord.count(); i++)
            newColumns.append(newRecord.field(i).name());

        QStringList commonColumns;
        for(const QString& newColumnName: std::as_const(newColumns)) {
            for(const QString& oldColumnName: std::as_const(oldColumns)) {
                driver()->escapeIdentifier(newColumnName, QSqlDriver::FieldName);
                if(QString::compare(oldColumnName, newColumnName, Qt::CaseInsensitive)==0)
                    commonColumns.append(this->prepareIdentifier(newColumnName, QSqlDriver::FieldName));
            }
        }
        commonColumns.removeDuplicates();

        sql = "INSERT INTO";
        sql = Sql::concat(sql, this->prepareIdentifier(m_into, QSqlDriver::TableName));
        sql = Sql::concat(sql, QString("(%1)").arg(commonColumns.join(',')));
        sql = Sql::concat(sql, QString("SELECT %1").arg(commonColumns.join(',')));
        sql = Sql::concat(sql, QString("FROM %1").arg(this->prepareIdentifier(m_table, QSqlDriver::TableName)));
    }
    else if(m_vacuum) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer)
        {
            QStringList queries;
            const QStringList existingTables = driver()->tables(QSql::Tables);
            for(const QString& table: existingTables)
            {
                const QString vacuumStmt = QString("OPTIMIZE TABLE %1").arg(this->prepareIdentifier(table, QSqlDriver::TableName));
                queries.append(vacuumStmt);
            }
            sql = queries.join(';');
            if(queries.size()>1)
                m_multiple = true;
        }
        else if(dbmsType==QSqlDriver::SQLite)
        {
            sql = "VACUUM";
        }
    }
    else if(m_analyze && !m_table.isEmpty()) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = "ANALYZE TABLE";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
        else if(dbmsType==QSqlDriver::SQLite) {
            sql = "ANALYZE";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
    }
    else if(m_version) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer)
            sql = "SELECT version() as VERSION";
        else if(dbmsType==QSqlDriver::SQLite)
            sql = "SELECT sqlite_version() as VERSION";
    }
    else if(m_autoIncrement && !m_table.isEmpty()) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = "SHOW TABLE STATUS LIKE";
            sql = Sql::concat(sql, this->formatValue(m_table));
        }
        else if(dbmsType==QSqlDriver::SQLite) {
            sql = "SELECT";
            sql = Sql::concat(sql, this->prepareIdentifier("seq", QSqlDriver::FieldName));
            sql = Sql::concat(sql, "as AUTO_INCREMENT");
            sql = Sql::concat(sql, Sql::from(this->prepareIdentifier("sqlite_sequence", QSqlDriver::TableName)));
            sql = Sql::concat(sql, Sql::where(SqlCondition("name").equals(m_table).connection(m_connection).build()));
        }
    }
    else if(m_indexList && !m_table.isEmpty()) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = "SHOW INDEX FROM";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
        else if(dbmsType==QSqlDriver::SQLite) {
            sql = "SELECT";
            sql = Sql::concat(sql, this->prepareIdentifier("name", QSqlDriver::FieldName));
            sql = Sql::concat(sql, "as Key_name");
            sql = Sql::concat(sql, QString("FROM pragma_index_list(%1)").arg(this->prepareIdentifier(m_table, QSqlDriver::TableName)));
        }
    }
    else if(m_tableCreation && !m_table.isEmpty()) {
        const QSqlDriver::DbmsType dbmsType = driver()->dbmsType();
        if(dbmsType==QSqlDriver::MySqlServer) {
            sql = "SHOW CREATE TABLE";
            sql = Sql::concat(sql, this->prepareIdentifier(m_table, QSqlDriver::TableName));
        }
        else if(dbmsType==QSqlDriver::SQLite) {
            sql = "SELECT";
            sql = Sql::concat(sql, this->prepareIdentifier("sql", QSqlDriver::FieldName));
            sql = Sql::concat(sql, "as CREATION_STMT");
            sql = Sql::concat(sql, Sql::from(this->prepareIdentifier("sqlite_master", QSqlDriver::TableName)));
            sql = Sql::concat(sql, Sql::where(SqlCondition("tbl_name").equals(m_table).connection(m_connection).build()));
        }
    }

    return sql;
}
