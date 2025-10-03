#ifndef SQLQUERYTABLEMODEL_H
#define SQLQUERYTABLEMODEL_H

#include "sqlquerymodel.h"
class SqlQueryTableModel : public SqlQueryModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_REF_PROPERTY(QString, primaryField, PrimaryField, "")

    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QString, rawFilter, RawFilter, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filterMap, FilterMap, {})
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, "")
    Q_WRITABLE_REF_PROPERTY(QString, filterPattern, FilterPattern, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_VAR_PROPERTY(int, limit, Limit, 0)
    Q_WRITABLE_VAR_PROPERTY(int, offset, Offset, 0)
    Q_WRITABLE_REF_PROPERTY(QString, sortRoleName, SortRoleName, "")
    Q_WRITABLE_VAR_PROPERTY(Qt::SortOrder, sortOrder, SortOrder, Qt::AscendingOrder)

public:
    explicit SqlQueryTableModel(QObject *parent = nullptr);

    QSqlRecord baseRecord() const override;

protected:
    QString queryStatement() override;

    virtual QString selectClause();
    virtual QString filterClause();
    virtual QString orderByClause();
    virtual QString limitClause();
    virtual QString offsetClause();

protected slots:
    void onTableNameChanged(const QString& tableName);

private:
    QSqlRecord m_baseRec;
    QSqlIndex m_primaryIndex;
};

#endif // SQLQUERYTABLEMODEL_H
