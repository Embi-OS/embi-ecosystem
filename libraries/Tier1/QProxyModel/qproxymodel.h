#ifndef QPROXYMODEL_H
#define QPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QQmlParserStatus>
#include <QItemSelection>
#include <QDefs>

#include "qvariantcompare.h"

struct QSortFilterProxyModelForeign {
    Q_GADGET
    QML_FOREIGN(QSortFilterProxyModel)
    QML_NAMED_ELEMENT(QSortFilterProxyModel)
};

class QProxyModel : public QSortFilterProxyModel,
                    public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ProxyModel)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_VAR_PROPERTY(bool, invalidateWhen, InvalidateWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, invalidateFilterWhen, InvalidateFilterWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, invalidateSorterWhen, InvalidateSorterWhen, true)

    Q_WRITABLE_VAR_PROPERTY(bool, filterEnabled, FilterEnabled, true)
    Q_WRITABLE_VAR_PROPERTY(int, filterColumn, FilterColumn, 0)
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, {})
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_REF_PROPERTY(QString, filterPattern, FilterPattern, {})

    Q_WRITABLE_VAR_PROPERTY(bool, sortEnabled, SortEnabled, true)
    Q_WRITABLE_VAR_PROPERTY(int, sortColumn, SortColumn, 0)
    Q_WRITABLE_REF_PROPERTY(QString, sortRoleName, SortRoleName, {})
    Q_WRITABLE_VAR_PROPERTY(Qt::SortOrder, sortOrder, SortOrder, Qt::AscendingOrder)

public:
    explicit QProxyModel(QObject* parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    QVariant data(const QModelIndex& index, int role) const final override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) final override;
    QHash<int, QByteArray> roleNames() const final override;

    QVariantMap sourceDatas(const QModelIndex& sourceIndex, const QStringList& roles = QStringList()) const;
    virtual QVariant sourceData(const QModelIndex& sourceIndex, const QString& roleName) const;
    virtual QVariant sourceData(const QModelIndex& sourceIndex, int role) const;
    virtual bool setSourceData(const QModelIndex& sourceIndex, const QVariant &value, const QString& roleName) const;
    virtual bool setSourceData(const QModelIndex& sourceIndex, const QVariant &value, int role) const;

    Q_INVOKABLE int roleForName(const QString& roleName) const;
    Q_INVOKABLE QByteArray roleName(int role) const;

    Q_INVOKABLE QModelIndexList sourceIndexes() const;
    Q_INVOKABLE QList<int> sourceRows() const;

    Q_INVOKABLE QModelIndex mapToSource(const QModelIndex& proxyIndex) const final override;
    Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex& sourceIndex) const final override;

    Q_INVOKABLE int mapToSource(int proxyRow) const;
    Q_INVOKABLE int mapFromSource(int sourceRow) const;

    Q_INVOKABLE QList<int> mapToSource(const QList<int>& proxyRows) const;
    Q_INVOKABLE QList<int> mapFromSource(const QList<int>& sourceRows) const;

    Q_INVOKABLE QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const final override;
    Q_INVOKABLE QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const final override;

    Q_INVOKABLE virtual bool isActive() const;

public Q_SLOTS:
    void setSourceModel(QAbstractItemModel *sourceModel) final override;

    virtual void queueInvalidate();
    virtual void invalidate();
    void queueInvalidateFilter();
    void invalidateFilter();
    void queueInvalidateSorter();
    void invalidateSorter();

Q_SIGNALS:
    void roleNamesChanged();
    void countChanged(int count);
    void emptyChanged(bool empty);

protected Q_SLOTS:
    void resetInternalData() override;
    void countInvalidate();
    virtual void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

    void updateFilterPattern();
    virtual void updateRoleNames();
    void updateFilterRole();
    void updateSortRole();
    virtual void updateRoles();
    void initRoles();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
    int compare(const QModelIndex& source_left, const QModelIndex& source_right) const;

    QHash<int, QByteArray> m_roleNames;
    QHash<QString, int> m_roleIds;

    bool m_isActive = false;
    bool m_invalidateQueued = false;
    bool m_invalidateFilterQueued = false;
    bool m_invalidateSorterQueued = false;

private:
    int m_count=0;
};

#endif // QPROXYMODEL_H
