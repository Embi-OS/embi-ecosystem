#ifndef SQLCMD_H
#define SQLCMD_H

#include <QVariant>
#include <QString>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QElapsedTimer>
#include <QDefs>
#include <QSqlDatabase>
#include <QDeferred>

#include "sql_helpertypes.h"

// helpers for building SQL expressions
class Sql : public QObject,
            public QQmlSingleton<Sql>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<Sql>;
    explicit Sql(QObject *parent = nullptr);

public:

    // SQL keywords
    Q_INVOKABLE static inline const QLatin1String inv() { return QLatin1String("NOT"); } // "not" is a C++ keyword
    Q_INVOKABLE static inline const QLatin1String add() { return QLatin1String("ADD"); }
    Q_INVOKABLE static inline const QLatin1String into() { return QLatin1String("INTO"); }
    Q_INVOKABLE static inline const QLatin1String values() { return QLatin1String("VALUES"); }
    Q_INVOKABLE static inline const QLatin1String drop() { return QLatin1String("DROP"); }
    Q_INVOKABLE static inline const QLatin1String rename() { return QLatin1String("RENAME"); }
    Q_INVOKABLE static inline const QLatin1String column() { return QLatin1String("COLUMN"); }
    Q_INVOKABLE static inline const QLatin1String as() { return QLatin1String("AS"); }
    Q_INVOKABLE static inline const QLatin1String collateNocase() { return QLatin1String("COLLATE NOCASE"); }
    Q_INVOKABLE static inline const QLatin1String lower() { return QLatin1String("LOWER"); }
    Q_INVOKABLE static inline const QLatin1String upper() { return QLatin1String("UPPER"); }
    Q_INVOKABLE static inline const QLatin1String asc() { return QLatin1String("ASC"); }
    Q_INVOKABLE static inline const QLatin1String comma() { return QLatin1String(","); }
    Q_INVOKABLE static inline const QLatin1String desc() { return QLatin1String("DESC"); }
    Q_INVOKABLE static inline const QLatin1String AND() { return QLatin1String("AND"); } // "and" is a C++ keyword
    Q_INVOKABLE static inline const QLatin1String OR() { return QLatin1String("OR"); } // "or" is a C++ keyword
    Q_INVOKABLE static inline const QLatin1String set() { return QLatin1String("SET"); }
    Q_INVOKABLE static inline const QLatin1String from() { return QLatin1String("FROM"); }
    Q_INVOKABLE static inline const QLatin1String leftJoin() { return QLatin1String("LEFT JOIN"); }
    Q_INVOKABLE static inline const QLatin1String limit() { return QLatin1String("LIMIT"); }
    Q_INVOKABLE static inline const QLatin1String offset() { return QLatin1String("OFFSET"); }
    Q_INVOKABLE static inline const QLatin1String on() { return QLatin1String("ON"); }
    Q_INVOKABLE static inline const QLatin1String orderBy() { return QLatin1String("ORDER BY"); }
    Q_INVOKABLE static inline const QLatin1String parenClose() { return QLatin1String(")"); }
    Q_INVOKABLE static inline const QLatin1String parenOpen() { return QLatin1String("("); }
    Q_INVOKABLE static inline const QLatin1String select() { return QLatin1String("SELECT"); }
    Q_INVOKABLE static inline const QLatin1String update() { return QLatin1String("UPDATE"); }
    Q_INVOKABLE static inline const QLatin1String distinct() { return QLatin1String("DISTINCT"); }
    Q_INVOKABLE static inline const QLatin1String sp() { return QLatin1String(" "); }
    Q_INVOKABLE static inline const QLatin1String where() { return QLatin1String("WHERE"); }
    Q_INVOKABLE static inline const QLatin1String groupBy() { return QLatin1String("GROUP BY"); }
    Q_INVOKABLE static inline const QLatin1String having() { return QLatin1String("HAVING"); }
    Q_INVOKABLE static inline const QLatin1String eq() { return QLatin1String("="); }
    Q_INVOKABLE static inline const QLatin1String greater() { return QLatin1String(">"); }
    Q_INVOKABLE static inline const QLatin1String in() { return QLatin1String("IN"); }
    Q_INVOKABLE static inline const QLatin1String count() { return QLatin1String("COUNT"); }
    Q_INVOKABLE static inline const QLatin1String avg() { return QLatin1String("AVG"); }
    Q_INVOKABLE static inline const QLatin1String sum() { return QLatin1String("SUM"); }
    Q_INVOKABLE static inline const QLatin1String min() { return QLatin1String("MIN"); }
    Q_INVOKABLE static inline const QLatin1String max() { return QLatin1String("MAX"); }

    // Build expressions based on key words
    Q_INVOKABLE static inline const QString concat(const QString &a, const QString &b) { return a.isEmpty() ? b : b.isEmpty() ? a : QString(a).append(sp()).append(b); }
    Q_INVOKABLE static inline const QString into(const QString &s) { return s.isEmpty() ? s : concat(into(), s); }
    Q_INVOKABLE static inline const QString values(const QString &s) { return s.isEmpty() ? s : concat(values(), s); }
    Q_INVOKABLE static inline const QString inv(const QString &s) { return s.isEmpty() ? s : concat(inv(), QString("(%1)").arg(s)); }
    Q_INVOKABLE static inline const QString as(const QString &a, const QString &b) { return b.isEmpty() ? a : concat(concat(a, as()), b); }
    Q_INVOKABLE static inline const QString collateNocase(const QString &s) { return s.isEmpty() ? s : concat(s, collateNocase()); }
    Q_INVOKABLE static inline const QString lower(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(lower(),s); }
    Q_INVOKABLE static inline const QString upper(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(upper(),s); }
    Q_INVOKABLE static inline const QString asc(const QString &s) { return s.isEmpty() ? s : concat(s, asc()); }
    Q_INVOKABLE static inline const QString comma(const QString &a, const QString &b) { return a.isEmpty() ? b : b.isEmpty() ? a : QString(a).append(comma()).append(b); }
    Q_INVOKABLE static inline const QString desc(const QString &s) { return s.isEmpty() ? s : concat(s, desc()); }
    Q_INVOKABLE static inline const QString AND(const QString &a, const QString &b) { return a.isEmpty() ? b : b.isEmpty() ? a : concat(concat(a, AND()), b); }
    Q_INVOKABLE static inline const QString from(const QString &s) { return s.isEmpty() ? s : concat(from(), s); }
    Q_INVOKABLE static inline const QString leftJoin(const QString &s) { return concat(leftJoin(), s); }
    Q_INVOKABLE static inline const QString limit(int i) { return i<=0 ? QString("") : concat(limit(), QString::number(i)); }
    Q_INVOKABLE static inline const QString offset(int i) { return i<=0 ? QString("") : concat(offset(), QString::number(i)); }
    Q_INVOKABLE static inline const QString on(const QString &s) { return concat(on(), s); }
    Q_INVOKABLE static inline const QString orderBy(const QString &s) { return s.isEmpty() ? s : concat(orderBy(), s); }
    Q_INVOKABLE static inline const QString paren(const QString &s) { return s.isEmpty() ? s : parenOpen() + s + parenClose(); }
    Q_INVOKABLE static inline const QString select(const QString &s) { return concat(select(), s); }
    Q_INVOKABLE static inline const QString update(const QString &s) { return concat(update(), s); }
    Q_INVOKABLE static inline const QString distinct(const QString &s) { return s.isEmpty() ? s : concat(distinct(), s); }
    Q_INVOKABLE static inline const QString where(const QString &s) { return s.isEmpty() ? s : concat(where(), s); }
    Q_INVOKABLE static inline const QString groupBy(const QString &s) { return s.isEmpty() ? s : concat(groupBy(), s); }
    Q_INVOKABLE static inline const QString having(const QString &s) { return s.isEmpty() ? s : concat(having(), s); }
    Q_INVOKABLE static inline const QString eq(const QString &a, const QString &b) { return QString(a).append(eq()).append(b); }
    Q_INVOKABLE static inline const QString greater(const QString &a, const QString &b) { return QString(a).append(greater()).append(b); }
    Q_INVOKABLE static inline const QString in(const QString &a, const QString &b) { return concat(a,concat(in(), b)); }
    Q_INVOKABLE static inline const QString count(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(count(),s); }
    Q_INVOKABLE static inline const QString avg(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(avg(),s); }
    Q_INVOKABLE static inline const QString sum(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(sum(),s); }
    Q_INVOKABLE static inline const QString min(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(min(),s); }
    Q_INVOKABLE static inline const QString max(const QString &s) { return s.isEmpty() ? s : QString("%1(%2)").arg(max(),s); }

    // SQL filter builder
    Q_INVOKABLE static bool isIfNullValid(const QString& name);
    Q_INVOKABLE static bool isValueTypeValid(const QString& name);
    Q_INVOKABLE static bool isFilterNameValid(const QString& name);
    Q_INVOKABLE static QString filterName(SqlFilterOperators::Enum op);
    Q_INVOKABLE static SqlFilterOperators::Enum filterOperator(const QString& name);
    Q_INVOKABLE static QString filterEquals(const QString &tableName, const QString &columnName, const QVariant &value, const QSqlDriver *driver=nullptr, bool preparedStatement=false);

    // SQL statements builder
    Q_INVOKABLE static QString prepareIdentifier(const QString &identifier, QSqlDriver::IdentifierType type, const QSqlDriver *driver=nullptr);
    Q_INVOKABLE static QVariant formatValue(const QVariant& value, SqlColumnTypes::Enum type, bool* ok=nullptr);
    Q_INVOKABLE static QString formatValue(QVariant value, const QSqlDriver *driver=nullptr, bool trimStrings = false);
    Q_INVOKABLE static QString primaryField(const QString &tableName, const QSqlDriver *driver=nullptr);
    static QSqlRecord prepareRecord(const QString &tableName, const QVariantMap& map, const QSqlDriver *driver=nullptr);

    Q_INVOKABLE static QString emptyInsertStatement(const QString &tableName, const QSqlDriver *driver=nullptr, SqlInsertStatementTypes::Enum insertType=SqlInsertStatementTypes::Insert);
    Q_INVOKABLE static QString valuesInsertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr);
    Q_INVOKABLE static QString insertMultipleStatement(const QString &tableName, const QVariantList &maps, const QSqlDriver *driver=nullptr, SqlInsertStatementTypes::Enum insertType=SqlInsertStatementTypes::Insert);
    Q_INVOKABLE static QString basicInsertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, SqlInsertStatementTypes::Enum insertType=SqlInsertStatementTypes::Insert);

    Q_INVOKABLE static QString selectStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString insertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString updateStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString whereStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString deleteWhereStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString sqlStatement(QSqlDriver::StatementType type, const QString &tableName, const QVariantMap &map, const QSqlDriver *driver=nullptr, bool preparedStatement=false);

    Q_INVOKABLE static QString emptyInsertStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr);
    Q_INVOKABLE static QString insertValueStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr);
    Q_INVOKABLE static QString insertStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString updateStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr, bool preparedStatement=false);
    Q_INVOKABLE static QString sqlStatement(QSqlDriver::StatementType type, const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver=nullptr, bool preparedStatement=false);

private:
    static void fillFilterMap();
    static QVariantMap m_filterMap;
};

#endif // SQLCMD_H
