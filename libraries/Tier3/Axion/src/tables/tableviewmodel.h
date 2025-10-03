#ifndef TABLEVIEWMODEL_H
#define TABLEVIEWMODEL_H

#include <QIdentityProxyModel>
#include <QQmlParserStatus>
#include <QDefs>

#include "tableviewcolumnmodel.h"

class TableViewModel: public QIdentityProxyModel,
                      public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)
    Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QString, verticalHeaderDisplayRoleName, VerticalHeaderDisplayRoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, verticalHeaderDisplayRole, VerticalHeaderDisplayRole, -1)
    Q_WRITABLE_REF_PROPERTY(QString, horizontalHeaderDisplayRoleName, HorizontalHeaderDisplayRoleName, "title")
    Q_WRITABLE_VAR_PROPERTY(int, horizontalHeaderDisplayRole, HorizontalHeaderDisplayRole, -1)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_PTR_PROPERTY(TableViewColumnModel, columnModel, ColumnModel, nullptr)

public:
    explicit TableViewModel(QObject* parent = nullptr);

    enum {
        // 0x0100 to 0x0999 Reserved for sourceModel role names
        // from 0x2000 is for columnModel role names
        ColumnFormattedRole = 0x1000,
        ColumnColorRole = 0x1001,
        ColumnObjectRole = 0x2000,
    };

    void classBegin() override;
    void componentComplete() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QColor formattedColor(const QModelIndex& index) const;
    QVariant formattedData(const QModelIndex& index) const;
    bool setFormattedData(const QModelIndex& index, const QVariant &value) const;

    virtual QVariantMap sourceDatas(int row) const;
    virtual QVariant sourceData(int row, int role) const;
    virtual bool setSourceData(int row, const QVariant &value, int role) const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    Q_INVOKABLE int roleForName(const QString& roleName) const;
    Q_INVOKABLE QByteArray roleName(int role) const;

    Q_INVOKABLE QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

    Q_INVOKABLE double columnWidth(int column) const;
    Q_INVOKABLE double implicitColumnWidth(int column) const;
    Q_INVOKABLE bool isColumnVisible(int column) const;
    Q_INVOKABLE bool isColumnResizable(int column) const;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void invalidate();

signals:
    void countChanged(int count);
    void columnCountChanged(int columnCount);
    void emptyChanged(bool empty);

protected slots:
    void resetInternalData() override;
    void countInvalidate();

    virtual void updateRoleNames();
    virtual void updateRoles();
    void initRoles();
    void initHeadersRole();

    void onColumnModelAboutToChange(TableViewColumnModel* oldColumnModel, TableViewColumnModel* newColumnModel);
    void onColumnModelChanged(TableViewColumnModel* columnModel);

    void onColumnDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void onSourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

    void queueChangeLayout();
    void changeLayout();

private:
    bool m_layoutChangeQueued=false;

    QHash<int, QByteArray> m_roleNames;
    QHash<QString, int> m_roleIds;

    int m_count=0;
    bool m_isActive=false;
};


#endif // TABLEVIEWMODEL_H
