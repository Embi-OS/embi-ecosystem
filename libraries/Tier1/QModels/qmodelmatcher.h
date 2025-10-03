#ifndef QMODELMATCHER_H
#define QMODELMATCHER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QQmlParserStatus>
#include <QQmlEngine>
#include <QDefs>

class QModelMatcher : public QObject,
                      public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModelMatcher)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(QModelIndexList indexes READ getIndexes NOTIFY indexesChanged FINAL)
    Q_WRITABLE_PTR_PROPERTY(QAbstractItemModel, sourceModel, SourceModel, nullptr)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

    Q_WRITABLE_REF_PROPERTY(QModelIndex, parentIndex, ParentIndex, QModelIndex())
    Q_WRITABLE_VAR_PROPERTY(int, startRow, StartRow, 0)
    Q_WRITABLE_VAR_PROPERTY(int, startColumn, StartColumn, 0)
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, Qt::DisplayRole)
    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})
    Q_WRITABLE_VAR_PROPERTY(int, hits, Hits, -1)
    Q_WRITABLE_VAR_PROPERTY(Qt::MatchFlags, flags, Flags, Qt::MatchExactly)

public:
    explicit QModelMatcher(QObject* parent = nullptr);

    void classBegin() override final;
    void componentComplete() override final;

    int count() const { return m_indexes.count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    const QModelIndexList& getIndexes() const;

public slots:
    void queueInvalidate();
    void invalidate();

signals:
    void aboutToBeInvalidated();
    void invalidated();

    void countChanged(int count);
    void emptyChanged(bool empty);

    void indexesChanged(const QModelIndexList& indexes);

protected slots:
    void onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel);
    void onModelChanged(QAbstractItemModel* model);

    void countInvalidate();

    void updateRoles();
    void initRoles();

    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles = QList<int>());
    void onModelRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void onModelColumnsInserted(const QModelIndex& parent, int first, int last);
    void onModelColumnsRemoved(const QModelIndex& parent, int first, int last);

private:
    QModelIndexList m_indexes;

    bool m_completed=false;
    int m_count=0;
    bool m_invalidateQueued = false;
};

#endif // QMODELMATCHER_H
