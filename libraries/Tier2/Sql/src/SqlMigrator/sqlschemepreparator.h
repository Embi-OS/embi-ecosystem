#ifndef SQLSCHEMEPREPARATOR_H
#define SQLSCHEMEPREPARATOR_H

#include "sqltablepreparator.h"
class SqlSchemePreparator : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_CONSTANT_OLP_PROPERTY(SqlTablePreparator, tables)
    Q_DEFAULT_PROPERTY(tables)

public:
    explicit SqlSchemePreparator(QObject* parent = nullptr);
    virtual bool isValid();

    bool run(const QString& dbConnectionName, bool canDrop=false);
    bool doRun(const QString& dbConnectionName, bool canDrop=false);

private:
    bool initTables();
    bool createTables();
    bool dropTables();
    bool updatePrimaries();

    bool fillDefaultEntriesTables();
    bool fillBasicEntriesTables();

    QString m_dbConnectionName = SqlDefaultConnection;
};

#endif // SQLSCHEMEPREPARATOR_H
