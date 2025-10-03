#ifndef SQLQUERYOBJECT_H
#define SQLQUERYOBJECT_H

#include "sqlquerymodel.h"
class SqlQueryObject : public SqlQueryModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_REF_PROPERTY(QString, primaryField, PrimaryField, "")

    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QString, rawFilter, RawFilter, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filterMap, FilterMap, {})
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, "")
    Q_WRITABLE_REF_PROPERTY(QString, filterPattern, FilterPattern, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)

public:
    explicit SqlQueryObject(QObject *parent = nullptr);

    QSqlRecord baseRecord() const override;
    bool isValid() const override;

    Q_INVOKABLE bool setValues(const QVariantMap& array);
    Q_INVOKABLE bool setValue(const QString& property, const QVariant& value);
    Q_INVOKABLE QVariantMap values(const QStringList& properties = QStringList()) const;
    Q_INVOKABLE QVariant value(const QString& property, const QVariant& defaultValue = QVariant()) const;

protected:
    QString queryStatement() override;

    virtual QString selectClause();
    virtual QString filterClause();

protected slots:
    void onTableNameChanged(const QString& tableName);

private:
    QSqlRecord m_baseRec;
    QSqlIndex m_primaryIndex;
};

#endif // SQLQUERYOBJECT_H
