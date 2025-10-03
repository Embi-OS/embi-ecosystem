#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QQmlParserStatus>

#include <QDefs>
#include "SqlCore/sqldbmanager.h"

class SqlTableModel : public QSqlTableModel,
                      public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_READONLY_REF_PROPERTY(QString, primaryField, PrimaryField, "")

    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QString, rawFilter, RawFilter, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filterMap, FilterMap, {})
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, "")
    Q_WRITABLE_REF_PROPERTY(QString, filterPattern, FilterPattern, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_VAR_PROPERTY(int, limit, Limit, 0)
    Q_WRITABLE_VAR_PROPERTY(int, offset, Offset, 0)
    Q_WRITABLE_VAR_PROPERTY(int, sortColumn, SortColumn, 0)
    Q_WRITABLE_REF_PROPERTY(QString, sortRoleName, SortRoleName, "")
    Q_WRITABLE_VAR_PROPERTY(Qt::SortOrder, sortOrder, SortOrder, Qt::AscendingOrder)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

public:
    explicit SqlTableModel(QObject *parent = nullptr);
    ~SqlTableModel() override;

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override final;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override final;

    QHash<int, QByteArray> roleNames() const final override;

    void classBegin() override;
    void componentComplete() override;

    Q_INVOKABLE int columnIndex(const QString&) const;
    Q_INVOKABLE QString columnName(int) const;

    Q_INVOKABLE virtual bool isActive() const;
    Q_INVOKABLE bool clearTable();
    Q_INVOKABLE bool remove(int index, int count=1);
    Q_INVOKABLE bool remove(const QList<int>& indexes);
    Q_INVOKABLE bool append(QJSValue value);
    bool append(const QVariantMap& row);
    bool append(const QVariantList& rows);
    QVariantMap prepareArray(const QVariantMap& map);

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

public slots:
    void invalidate();
    void queueInvalidate();

signals:
    void aboutToBeInvalidated();
    void invalidated();

    void countChanged(int count);
    void emptyChanged(bool empty);

protected slots:
    bool select() override final;
    bool submit() override final;
    void queueSubmit();
    void countInvalidate();

protected:
    QString selectStatement() const override final;

    QHash<int, QByteArray> m_roleNames;

private:
    int m_dirtyRow = -1;

    bool m_completed = true;
    bool m_selectQueued = false;
    bool m_submitQueued = false;

    int m_count=0;
};

#endif // SQLTABLEMODEL_H
