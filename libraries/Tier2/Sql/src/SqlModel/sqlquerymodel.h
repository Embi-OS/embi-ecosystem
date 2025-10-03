#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QQmlParserStatus>

#include <QDefs>
#include "SqlCore/sqldbmanager.h"

class SqlQueryModel : public QSqlQueryModel,
                      public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QString, rawStatement, RawStatement, "")
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_READONLY_VAR_PROPERTY(bool, isQuerying, IsQuerying, false)

public:
    explicit SqlQueryModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &item, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const final override;

    QSqlDatabase database() const;
    const QSqlDriver* driver() const;
    virtual QSqlRecord baseRecord() const;

    void classBegin() override;
    void componentComplete() override;

    Q_INVOKABLE int columnIndex(const QString&) const;
    Q_INVOKABLE QString columnName(int) const;

    Q_INVOKABLE void markDirty();
    Q_INVOKABLE virtual bool isActive() const;
    Q_INVOKABLE virtual bool isValid() const;

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
    virtual bool select();
    void countInvalidate();

protected:
    void queryChange() override;
    virtual QString queryStatement();

private:
    bool setRawQuery(const QString &query);

    QHash<int, QByteArray> m_roleNames;

    bool m_isActive = false;
    bool m_isValid = false;
    bool m_selectQueued = false;

    int m_count=0;
};

#endif // SQLQUERYMODEL_H
