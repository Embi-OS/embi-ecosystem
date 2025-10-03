#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <Sql>
#include <Axion>
class SqlManager : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(SqlManager)

    Q_WRITABLE_REF_PROPERTY(QString, preparatorModule, PreparatorModule, "")
    Q_WRITABLE_REF_PROPERTY(QString, preparatorName, PreparatorName, "")

    Q_WRITABLE_VAR_PROPERTY(SqlDatabaseTypes::Enum, dbType, DbType, SqlDatabaseTypes::None)
    Q_WRITABLE_REF_PROPERTY(QString, dbName, DbName, "")
    Q_WRITABLE_REF_PROPERTY(QString, dbPath, DbPath, "")
    Q_WRITABLE_REF_PROPERTY(QString, dbServer, DbServer, "")
    Q_WRITABLE_REF_PROPERTY(QString, dbUserName, DbUserName, "")
    Q_WRITABLE_REF_PROPERTY(QString, dbPassword, DbPassword, "")
    Q_WRITABLE_VAR_PROPERTY(int, dbPort, DbPort, 3306)
    Q_WRITABLE_REF_PROPERTY(QString, dbConnectOptions, DbConnectOptions, "MYSQL_OPT_RECONNECT=1; MYSQL_OPT_CONNECT_TIMEOUT=3")

private:
    explicit SqlManager(QObject *parent = nullptr);

public:
    bool init() final override;
    bool unInit() final override;

    void setPreparator(const QString& module, const QString& name);

    SqlDbProfile createConnectionProfile();

    Q_INVOKABLE static void testConnexion(const QVariantMap& params);
    Q_INVOKABLE void dropDatabase();
    Q_INVOKABLE void vacuumDatabase();
    Q_INVOKABLE void exportDatabase(const QString& path);
    Q_INVOKABLE void importDatabase(const QString& path);
};

#endif // SQLMANAGER_H
