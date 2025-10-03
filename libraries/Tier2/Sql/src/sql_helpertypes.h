#ifndef SQL_HELPERTYPES_H
#define SQL_HELPERTYPES_H

#include <QVariant>
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlIndex>
#include <QDateTime>
#include <QElapsedTimer>

#include <QDefs>
#include <QUtils>

#define SqlDefaultConnection        (QLatin1String(QSqlDatabase::defaultConnection))
#define SqlAsyncConnectionPrefix    (QStringLiteral("async_"))

Q_ENUM_CLASS(SqlSyncTypes, SqlSyncType,
             Synchronous,
             Asynchronous,
             SemiSynchronous,)

Q_ENUM_CLASS(SqlInsertStatementTypes, SqlInsertStatementType,
             Insert,
             InsertOrReplace,
             InsertOrIgnore,)

Q_ENUM_CLASS(SqlJoinTypes, SqlJoinType,
             Inner,
             Left,
             Right,
             Cross)

Q_ENUM_CLASS(SqlDatabaseTypes, SqlDatabaseType,
             None,
             SQLite,
             MySQL,)

Q_ENUM_CLASS(SqlColumnTypes, SqlColumnType,
             Invalid,
             Char,
             VarChar,
             Binary,
             VarBinary,
             TinyText,
             Text,
             MediumText,
             LongText,
             Blob,
             Boolean,
             SmallInt,
             Integer,
             BigInt,
             Float,
             DoublePrecision,
             Date,
             Time,
             DateTime,
             Json,
             Uuid,
             Timestamp,)

Q_FLAG_CLASS(SqlColumnOptions, SqlColumnOption, SqlColumnOptionEnum,
             None               = 0,
             NotNullable        = (1<<0),
             Unique             = (1<<1),
             AutoIncrement      = (1<<2),
             Primary            = (1<<3) | Unique,)

Q_ENUM_CLASS(SqlFilterOperators, SqlFilterOperator,
             Equals,
             NotEquals,
             LessThan,
             LessEquals,
             GreaterThan,
             GreaterEquals,
             Range,
             In,
             IContains,
             IStartsWith,
             IEndsWith,
             Contains,
             StartsWith,
             EndsWith,
             RegExp)

Q_ENUM_CLASS(SqlAggregateTypes, SqlAggregateType,
             None,
             Count,
             Avg,
             Sum,
             Min,
             Max)

struct QtSqlDriverForeign {
    Q_GADGET
    QML_FOREIGN(QSqlDriver)
    QML_NAMED_ELEMENT(SqlDriver)
    QML_UNCREATABLE("Abstract")
};

class SqlHelper : public QObject,
                  public QQmlSingleton<SqlHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<SqlHelper>;
    explicit SqlHelper(QObject *parent = nullptr);

public:
};

#endif // SQL_HELPERTYPES_H
