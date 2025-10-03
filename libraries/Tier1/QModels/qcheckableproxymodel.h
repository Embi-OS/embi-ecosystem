#ifndef QCHECKABLEPROXYMODEL_H
#define QCHECKABLEPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QItemSelection>
#include <QDefs>

class QCheckableProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(CheckableProxyModel)

    Q_COMPOSITION_PROPERTY(QItemSelectionModel, selection, nullptr)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, inverted, Inverted, false)

    Q_PROPERTY(int selectionCount READ selectionCount NOTIFY selectionChanged FINAL)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QModelIndexList selectedIndexes READ selectedIndexes NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QList<int> selectedRows READ selectedRows WRITE setSelectedRows NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QList<int> selectedColumns READ selectedColumns WRITE setSelectedColumns NOTIFY selectionChanged FINAL)

public:
    explicit QCheckableProxyModel(QObject * parent = nullptr);

    bool setData(const QModelIndex& modelIndex, const QVariant& value, int role) override;
    QVariant data(const QModelIndex& modelIndex, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    int selectionCount() const;
    bool hasSelection() const;
    QModelIndexList selectedIndexes() const;

    QList<int> selectedRows() const;
    void setSelectedRows(QList<int> rows);

    QList<int> selectedColumns() const;
    void setSelectedColumns(QList<int> columns);

    Q_INVOKABLE bool isChecked(int index) const;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

public slots:
    void clear();
    void selectAll();

    void set(const QModelIndex &index, bool select);
    void setRow(int row, bool select);
    void setColumn(int column, bool select);

    void select(const QModelIndex &index);
    void selectRow(int row);
    void selectColumn(int column);

    void deselect(const QModelIndex &index);
    void deselectRow(int row);
    void deselectColumn(int column);

    void toggle(const QModelIndex &index);
    void toggleRow(int row);
    void toggleColumn(int column);

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);

    void selectionChanged();

protected slots:
    void countInvalidate();

    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void resetInternalData() override;
    void updateRoleNames();
    void initRoles();

private:
    QHash<int, QByteArray> m_roleNames;
    int m_count=0;

    bool m_isUpdatingFromProperty=false;

    QList<int> m_selectedRows;
    QList<int> m_selectedColumns;
};

#endif // QCHECKABLEPROXYMODEL_H
