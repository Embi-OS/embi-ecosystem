#ifndef SQLBROWSER_H
#define SQLBROWSER_H

#include <QQmlParserStatus>
#include <QStringListModel>
#include <QSqlTableModel>

#include <QDefs>
#include "SqlCore/sqldbmanager.h"

class SqlBrowser : public QObject,
                   public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES(QQmlParserStatus)
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)

    Q_CONSTANT_PTR_PROPERTY(QStringListModel, tableNames, TableNames, nullptr)
    Q_CONSTANT_PTR_PROPERTY(QStringListModel, connectionNames, ConnectionNames, nullptr)
    Q_READONLY_PTR_PROPERTY(QAbstractItemModel, model, Model, nullptr)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_READONLY_VAR_PROPERTY(bool, editable, Editable, false)

public:
    explicit SqlBrowser(QObject *parent = nullptr);

    QSqlDatabase database() const;
    const QSqlDriver* driver() const;

    void classBegin() override;
    void componentComplete() override;

public slots:
    void invalidate();
    void queueInvalidate();

    void showTable(const QString &table);
    void showMetaData(const QString &table);

signals:
    void statusMessage(const QString &message);

private:
    bool m_selectQueued = false;
};

#endif // SQLBROWSER_H
