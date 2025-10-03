#ifndef SQLDBMANAGER_H
#define SQLDBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDeferred>

#include "SqlMigrator/sqldbpreparator.h"

class SqlDbManager : public QObject,
                     public QQmlSingleton<SqlDbManager>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<SqlDbManager>;
    explicit SqlDbManager(QObject *parent = nullptr);

public:
    QDeferred<QSqlError> testConnexion(const SqlDbProfile& profile);
    QDefer dropDatabase(const QString& connectionName=SqlDefaultConnection);

    Q_INVOKABLE bool open(SqlDbPreparator* preparator);
    Q_INVOKABLE bool close(const QString& connectionName=SqlDefaultConnection);

    bool openConnection(const SqlDbProfile& profile);

private:
    bool openConnection(SqlDbPreparator* preparator);
    bool createDatabase(SqlDbPreparator* preparator);
};

#endif // SQLDBMANAGER_H
