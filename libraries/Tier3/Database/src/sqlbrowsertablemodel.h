#ifndef SQLBROWSERTABLEMODEL_H
#define SQLBROWSERTABLEMODEL_H

#include <QDefs>
#include <Sql>

Q_ENUM_CLASS(SqlBrowserDataTypes, SqlBrowserDataType,
             None,
             Boolean,
             Bytes,
             Number,
             Text,)

class SqlBrowserTableModel: public QSqlTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit SqlBrowserTableModel(QObject* parent=nullptr, QSqlDatabase db = QSqlDatabase());

    enum {
        DisplayRole = Qt::DisplayRole,
        EditRole = Qt::EditRole,
        DirtyRole = Qt::UserRole,
        FieldnameRole,
        TypeRole,
        LengthRole,
        PrecisionRole,
        RequiredRole,
        AutoValueRole,
        DefaultValueRole,
    };

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override final;

    QHash<int, QByteArray> roleNames() const final override;

    Q_INVOKABLE bool hasChanged() const;

    Q_INVOKABLE static SqlBrowserDataTypes::Enum dataType(const QString& metaType);

public slots:
    bool select() final override;

private:
    QHash<int, QByteArray> m_roleNames;
};

#endif // SQLBROWSERTABLEMODEL_H
