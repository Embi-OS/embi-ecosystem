#ifndef QSELECTIONHELPER_H
#define QSELECTIONHELPER_H

#include <QItemSelectionModel>
#include <QQmlEngine>
#include <QDefs>

class QSelectionModel: public QItemSelectionModel
{
public:
    enum CurrentIndexPolicyFlag {
        CurrentIndexPolicyDefault        = 0x0000,
        CurrentIndexPolicyClear          = 0x0001,
        CurrentIndexPolicyToggleCell     = 0x0002,
        CurrentIndexPolicyToggleRow      = 0x0004 | CurrentIndexPolicyToggleCell,
        CurrentIndexPolicyToggleColumn   = 0x0008 | CurrentIndexPolicyToggleCell,
        CurrentIndexPolicyToggle         = CurrentIndexPolicyToggleRow | CurrentIndexPolicyToggleColumn
    };
    Q_DECLARE_FLAGS(CurrentIndexPolicyFlags, CurrentIndexPolicyFlag)
    Q_FLAG(CurrentIndexPolicyFlags)

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(SelectionModel)

    Q_WRITABLE_VAR_PROPERTY(bool, currentIndexEnabled, CurrentIndexEnabled, true)
    Q_WRITABLE_VAR_PROPERTY(QSelectionModel::CurrentIndexPolicyFlags, currentIndexPolicy, CurrentIndexPolicy, QSelectionModel::CurrentIndexPolicyDefault)

public:
    explicit QSelectionModel(QAbstractItemModel* parent = nullptr);

public slots:
    void setCurrentIndex(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) override;
    void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) override;
    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) override;
    void clear() override;
    void reset() override;

    void clearCurrentIndex() override;
};

class QSelectionHelper: public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SelectionHelper)
    QML_UNCREATABLE("Attached")
    QML_ATTACHED(QSelectionHelper)

    // TODO: add some usefull property

    // ──────── CONSTRUCTOR ──────────
private:
    explicit QSelectionHelper(QItemSelectionModel* object);

public:
    static QSelectionHelper* wrap(const QObject* object);
    static QSelectionHelper* qmlAttachedProperties(QObject* object);

    // ──────── COUNT PUBLIC API ──────────
private:
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QModelIndexList selectedIndexes READ selectedIndexes NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QList<int> selectedRows READ selectedRows WRITE setSelectedRows NOTIFY selectionChanged FINAL)
    Q_PROPERTY(QList<int> selectedColumns READ selectedColumns WRITE setSelectedColumns NOTIFY selectionChanged FINAL)

protected slots:
    void countInvalidate();

    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);

    void selectionChanged();

    // ──────── PUBLIC API ──────────
public:
    int count() const { return m_selection->selectedIndexes().count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex());

    bool hasSelection() const;
    QModelIndexList selectedIndexes() const;

    QList<int> selectedRows(int column=0) const;
    void setSelectedRows(QList<int> rows);

    QList<int> selectedColumns(int row=0) const;
    void setSelectedColumns(QList<int> columns);

    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void deselectAll();
    Q_INVOKABLE void toggleAll();

    Q_INVOKABLE void set(const QModelIndex &index, bool select);
    Q_INVOKABLE void setRow(int row, bool select);
    Q_INVOKABLE void setColumn(int column, bool select);

    Q_INVOKABLE void select(const QModelIndex &index);
    Q_INVOKABLE void selectRow(int row);
    Q_INVOKABLE void selectColumn(int column);

    Q_INVOKABLE void deselect(const QModelIndex &index);
    Q_INVOKABLE void deselectRow(int row);
    Q_INVOKABLE void deselectColumn(int column);

    Q_INVOKABLE void toggle(const QModelIndex &index);
    Q_INVOKABLE void toggleRow(int row);
    Q_INVOKABLE void toggleColumn(int column);

    Q_INVOKABLE void setCurrentIndex(const QModelIndex &index);
    Q_INVOKABLE void setCurrentRow(int row);
    Q_INVOKABLE void setCurrentColumn(int column);

    // ──────── PUBLIC STATIC API ──────────
public:
    static QModelIndex index(QItemSelectionModel* selection, int row, int column, const QModelIndex &parent=QModelIndex());

    static QList<int> selectedRows(QItemSelectionModel* selection, int column=0);
    static QList<int> selectedColumns(QItemSelectionModel* selection, int row=0);

    static void selectAll(QItemSelectionModel* selection);
    static void deselectAll(QItemSelectionModel* selection);
    static void toggleAll(QItemSelectionModel* selection);

    static void set(QItemSelectionModel* selection, const QModelIndex &index, bool select);
    static void setRow(QItemSelectionModel* selection, int row, bool select);
    static void setColumn(QItemSelectionModel* selection, int column, bool select);

    static void select(QItemSelectionModel* selection, const QModelIndex &index);
    static void selectRow(QItemSelectionModel* selection, int row);
    static void selectColumn(QItemSelectionModel* selection, int column);

    static void deselect(QItemSelectionModel* selection, const QModelIndex &index);
    static void deselectRow(QItemSelectionModel* selection, int row);
    static void deselectColumn(QItemSelectionModel* selection, int column);

    static void toggle(QItemSelectionModel* selection, const QModelIndex &index);
    static void toggleRow(QItemSelectionModel* selection, int row);
    static void toggleColumn(QItemSelectionModel* selection, int column);

    static void setCurrentIndex(QItemSelectionModel* selection, const QModelIndex &index);
    static void setCurrentRow(QItemSelectionModel* selection, int row);
    static void setCurrentColumn(QItemSelectionModel* selection, int column);

    // ──────── ATTRIBUTES ──────────
private:
    QItemSelectionModel* m_selection = nullptr;
    int m_count=0;

    bool m_isUpdatingFromProperty=false;
};

#endif // QSELECTIONHELPER_H
