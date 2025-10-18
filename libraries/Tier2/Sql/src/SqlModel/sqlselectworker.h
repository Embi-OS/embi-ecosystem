#ifndef SQLSELECTWORKER_H
#define SQLSELECTWORKER_H

#include "sql_helpertypes.h"
#include "SqlCore/sqlquerybuilder.h"
#include "syncable/qsworker.h"
class SqlSelectWorker : public QSWorker
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")

    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filters, Filters, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, sorters, Sorters, {})

    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})

    Q_WRITABLE_REF_PROPERTY(QList<SqlJoinQuery>, joins, Joins, {})

    Q_WRITABLE_VAR_PROPERTY(int, limit, Limit, 0)
    Q_WRITABLE_VAR_PROPERTY(int, offset, Offset, 0)

    Q_WRITABLE_VAR_PROPERTY(int, perPage, PerPage, 0)
    Q_WRITABLE_VAR_PROPERTY(int, page, Page, 0)

public:
    explicit SqlSelectWorker(QObject *parent = nullptr);

public slots:
    bool doRun() final override;
    bool abort() final override;
    bool waitForFinished(int timeout = -1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) final override;
};

#endif // SQLSELECTWORKER_H
