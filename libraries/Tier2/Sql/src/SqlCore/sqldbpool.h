#ifndef SQLDBPOOL_H
#define SQLDBPOOL_H

#include "sqldbprofile.h"
#include "../sql_helpertypes.h"

class SqlDbPool
{
public:
    explicit SqlDbPool();

    static bool init(const SqlDbProfile& profile, int retry=0);
    static QSqlDatabase create(const SqlDbProfile& profile, bool force=false);
    static bool closeDatabase(const QString& connection=SqlDefaultConnection);
    static bool closeThread(const QString& suffix);

    static QString connectionName(const QString& connection=SqlDefaultConnection);
    static QSqlDatabase database(const QString& connection=SqlDefaultConnection, bool open = true);
    static QSqlDatabase defaultDatabase(bool open = true);
    static QSqlDriver* driver(const QString& connection=SqlDefaultConnection);
    static QSqlDriver* defaultDriver();

    static QStringList drivers();
};

#endif // SQLDBPOOL_H
