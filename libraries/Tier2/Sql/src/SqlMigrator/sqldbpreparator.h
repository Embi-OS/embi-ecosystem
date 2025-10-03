#ifndef SQLDBPREPARATOR_H
#define SQLDBPREPARATOR_H

#include "SqlCore/sqldbprofile.h"

#include "sqlschemepreparator.h"
class SqlDbPreparator : public SqlSchemePreparator
{
    Q_OBJECT
    QML_ELEMENT

    // Generic
    Q_WRITABLE_VAR_PROPERTY(SqlDatabaseTypes::Enum, type, Type, SqlDatabaseTypes::None)
    Q_WRITABLE_REF_PROPERTY(QString, name, Name, "")
    Q_WRITABLE_REF_PROPERTY(QString, connectionName, ConnectionName, SqlDefaultConnection)
    Q_WRITABLE_VAR_PROPERTY(int, timeout, Timeout, 10000)

    // SQLite
    Q_WRITABLE_REF_PROPERTY(QString, path, Path, "")

    // MySQL
    Q_WRITABLE_REF_PROPERTY(QString, server, Server, "")
    Q_WRITABLE_REF_PROPERTY(QString, userName, UserName, "")
    Q_WRITABLE_REF_PROPERTY(QString, password, Password, "")
    Q_WRITABLE_VAR_PROPERTY(int, port, Port, 3306)
    Q_WRITABLE_REF_PROPERTY(QString, connectOptions, ConnectOptions, "MYSQL_OPT_RECONNECT=1; MYSQL_OPT_CONNECT_TIMEOUT=3")

    Q_WRITABLE_VAR_PROPERTY(bool, canDrop, CanDrop, false)

public:
    explicit SqlDbPreparator(QObject* parent = nullptr);
    bool isValid() override;

    bool execute();

    SqlDbProfile createConnectionProfile();
    QString toString();
};

#endif // SQLDBPREPARATOR_H
