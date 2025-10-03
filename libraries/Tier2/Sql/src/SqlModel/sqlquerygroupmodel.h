#ifndef SQLQUERYGROUPMODEL_H
#define SQLQUERYGROUPMODEL_H

#include "sqlquerymodel.h"
class SqlQueryGroupModel : public SqlQueryModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QString, rawFilter, RawFilter, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filterMap, FilterMap, {})
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, "")
    Q_WRITABLE_REF_PROPERTY(QString, filterPattern, FilterPattern, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_REF_PROPERTY(QString, groupRoleName, GroupRoleName, "")

public:
    explicit SqlQueryGroupModel(QObject *parent = nullptr);

protected:
    QString queryStatement() override;

    virtual QString selectClause();
    virtual QString filterClause();
    virtual QString groupByClause();
};

#endif // SQLQUERYGROUPMODEL_H
