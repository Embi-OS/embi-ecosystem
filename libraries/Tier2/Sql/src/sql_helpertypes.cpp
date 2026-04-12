#include "sql_helpertypes.h"

SqlHelper::SqlHelper(QObject *parent) :
    QObject(parent)
{

}

QString SqlHelper::sqlTypeGenerated(SqlColumnType type, int precision)
{
    switch (type) {
    case SqlColumnTypes::Char:
        return QString("CHAR(%1)").arg(precision);
    case SqlColumnTypes::VarChar:
        return QString("VARCHAR(%1)").arg(precision);
    case SqlColumnTypes::Binary:
        return QString("BINARY(%1)").arg(precision);
    case SqlColumnTypes::VarBinary:
        return QString("VARBINARY(%1)").arg(precision);
    case SqlColumnTypes::TinyText:
        return QStringLiteral("TINYTEXT");
    case SqlColumnTypes::Text:
        return QStringLiteral("TEXT");
    case SqlColumnTypes::MediumText:
        return QStringLiteral("MEDIUMTEXT");
    case SqlColumnTypes::LongText:
        return QStringLiteral("LONGTEXT");
    case SqlColumnTypes::Json:
        return QStringLiteral("JSON");
    case SqlColumnTypes::Uuid:
        return QStringLiteral("CHAR(36)");
    case SqlColumnTypes::Blob:
        return QStringLiteral("BLOB");
    case SqlColumnTypes::Boolean:
        return QStringLiteral("BOOLEAN");
    case SqlColumnTypes::SmallInt:
        return QStringLiteral("SMALLINT");
    case SqlColumnTypes::Integer:
        return QStringLiteral("INTEGER");
    case SqlColumnTypes::BigInt:
        return QStringLiteral("BIGINT");
    case SqlColumnTypes::Float:
        return QStringLiteral("FLOAT");
    case SqlColumnTypes::DoublePrecision:
        return QStringLiteral("DOUBLE PRECISION");
    case SqlColumnTypes::Date:
        return QStringLiteral("DATE");
    case SqlColumnTypes::Time:
        return QStringLiteral("TIME");
    case SqlColumnTypes::DateTime:
        return QStringLiteral("DATETIME");
    case SqlColumnTypes::Timestamp:
        return QStringLiteral("TIMESTAMP");
    default:
        return QString();
    }
}
