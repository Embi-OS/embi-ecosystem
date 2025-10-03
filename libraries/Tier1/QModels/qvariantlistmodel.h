#ifndef QVARIANTLISTMODEL_H
#define QVARIANTLISTMODEL_H

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QList>
#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QDebug>
#include <QDefs>

#include "syncable/qspatchable.h"
#include "syncable/qspatch.h"

Q_ENUM_CLASS(QVariantListModelPolicies, QVariantListModelPolicy,
             Direct,
             Delayed,
             Postponed,
             Manual,
             Disabled)

class QVariantListModel: public QAbstractListModel,
                         public QSPatchable,
                         public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VariantListModel)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(QVariantList storage READ getStorage NOTIFY storageChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QString, baseName, BaseName, {})
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, roles, Roles, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, placeholder, Placeholder, {})
    Q_WRITABLE_REF_PROPERTY(QVariant, source, Source, {})

    Q_WRITABLE_VAR_PROPERTY(bool, syncable, Syncable, false)
    Q_WRITABLE_VAR_PROPERTY(bool, backupable, Backupable, true)
    Q_WRITABLE_VAR_PROPERTY(QVariantListModelPolicies::Enum, selectPolicy, SelectPolicy, QVariantListModelPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantListModelPolicies::Enum, submitPolicy, SubmitPolicy, QVariantListModelPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantListModelPolicies::Enum, revertPolicy, RevertPolicy, QVariantListModelPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantListModelPolicies::Enum, updatePolicy, UpdatePolicy, QVariantListModelPolicies::Direct)

    Q_WRITABLE_VAR_PROPERTY(bool, selectWhen, SelectWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, submitWhen, SubmitWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, revertWhen, RevertWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, updateWhen, UpdateWhen, true)

    Q_WRITABLE_VAR_PROPERTY(int, selectInhibitTime, SelectInhibitTime, 0)
    Q_WRITABLE_VAR_PROPERTY(int, submitInhibitTime, SubmitInhibitTime, 0)
    Q_WRITABLE_VAR_PROPERTY(int, revertInhibitTime, RevertInhibitTime, 0)
    Q_WRITABLE_VAR_PROPERTY(int, updateInhibitTime, UpdateInhibitTime, 0)

    Q_READONLY_VAR_PROPERTY(bool, isDirty, IsDirty, false)
    Q_READONLY_VAR_PROPERTY(bool, selecting, Selecting, false)
    Q_READONLY_VAR_PROPERTY(bool, submiting, Submiting, false)
    Q_READONLY_VAR_PROPERTY(bool, reverting, Reverting, false)
    Q_READONLY_VAR_PROPERTY(bool, updating, Updating, false)
    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)
    Q_READONLY_VAR_PROPERTY(bool, selected, Selected, false)

    Q_READONLY_VAR_PROPERTY(bool, selectQueued, SelectQueued, false)
    Q_READONLY_VAR_PROPERTY(bool, submitQueued, SubmitQueued, false)
    Q_READONLY_VAR_PROPERTY(bool, revertQueued, RevertQueued, false)
    Q_READONLY_VAR_PROPERTY(bool, updateQueued, UpdateQueued, false)

    Q_READONLY_VAR_PROPERTY(bool, selectPostponed, SelectPostponed, false)
    Q_READONLY_VAR_PROPERTY(bool, submitPostponed, SubmitPostponed, false)
    Q_READONLY_VAR_PROPERTY(bool, revertPostponed, RevertPostponed, false)
    Q_READONLY_VAR_PROPERTY(bool, updatePostponed, UpdatePostponed, false)

    // ──────── CONSTRUCTOR ──────────
public:
    explicit QVariantListModel(QObject* parent = nullptr);
    explicit QVariantListModel(const QString& baseName, QObject* parent = nullptr);
    ~QVariantListModel() override;

    static const QModelIndex& noParent();

    // ──────── ABSTRACT MODEL OVERRIDE ──────────
public:
    QVariant data(const QModelIndex& modelIndex, int role) const override final;
    bool setData(const QModelIndex& modelIndex, const QVariant& value, int role) override final;

    QHash<int, QByteArray> roleNames() const override final;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override final;

    void shuffle();
    static void shuffle(QVariantList& storage);

    void sort(int role, Qt::SortOrder order = Qt::AscendingOrder) override;
    void sort(const QString& roleName, Qt::SortOrder order = Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static void sort(QVariantList& storage, const QString& roleName, Qt::SortOrder order = Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);

    int indexOf(int role, const QVariant& val, bool isSorted=false, Qt::SortOrder order=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    int indexOf(const QString& roleName, const QVariant& val, bool isSorted=false, Qt::SortOrder order=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    static int indexOf(const QVariantList& storage, const QString &roleName, const QVariant& val, bool isSorted=false, Qt::SortOrder order=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);

    // ──────── PUBLIC API ──────────
public:
    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    Q_INVOKABLE bool append(const QJSValue& value);
    Q_INVOKABLE bool prepend(const QJSValue& value);
    Q_INVOKABLE bool insert(int index, const QJSValue& value);
    bool append(const QVariant& variant);
    bool prepend(const QVariant& variant);
    bool insert(int index, const QVariant& variant) final override;

    Q_INVOKABLE QVariant at(int index) const;
    Q_INVOKABLE QVariant get(int index) const;
    Q_INVOKABLE bool contains(const QVariant& variant) const;
    Q_INVOKABLE bool set(int index, const QVariant& variant) final override;
    Q_INVOKABLE bool move(int from, int to, int count = 1) final override;
    Q_INVOKABLE bool moveUp(int index);
    Q_INVOKABLE bool moveDown(int index);
    Q_INVOKABLE bool remove(int index, int count = 1) final override;
    Q_INVOKABLE bool remove(QList<int> indexes);
    Q_INVOKABLE bool clear();
    Q_INVOKABLE void alter(const QSPatch &patch);

    Q_INVOKABLE int roleForName(const QString& name) const;
    Q_INVOKABLE QByteArray roleName(int role) const;
    void clearRoleNames();
    void updateRoleNames(const QVariant& var);

    const QVariantList& getStorage() const;
    const QVariantList& getBackup() const;

public slots:
    void queueSelect();
    bool select();
    virtual bool waitForSelect(int timeout=-1);

    void queueSubmit();
    bool submit() override final;
    virtual bool waitForSubmit(int timeout=-1);

    void queueRevert();
    void revert() override final;
    virtual bool waitForRevert(int timeout=-1);

    void queueUpdate();
    void update();
    virtual bool waitForUpdate(int timeout=-1);

    bool cancel();

signals:
    void completedChanged(bool completed);

    void countChanged(int count);
    void emptyChanged(bool empty);
    void storageChanged(const QVariantList& storage);

    void selectDone(bool result);
    void submitDone(bool result);
    void revertDone(bool result);
    void updateDone(bool result);

protected slots:
    void emitSelectDone(bool result);
    void emitSubmitDone(bool result);
    void emitRevertDone(bool result);
    void emitUpdateDone(bool result);

    void onSelectInhibitTimeChanged(int inhibitTime);
    void onSubmitInhibitTimeChanged(int inhibitTime);
    void onRevertInhibitTimeChanged(int inhibitTime);
    void onUpdateInhibitTimeChanged(int inhibitTime);

protected:
    void countInvalidate();
    void contentInvalidate();

    bool setStorage(const QVariantList& storage);
    bool setStorage(QVariantList&& storage);

    bool replaceStorage(const QVariantList& storage);
    bool replaceStorage(QVariantList&& storage);

    bool patchStorage(const QVariantList& storage);
    bool patchStorage(QVariantList&& storage);

    virtual bool doSelect();
    virtual bool doSubmit();
    virtual bool doRevert();
    virtual bool doUpdate();
    virtual bool doCancel();

    virtual QVariantList selectSource(bool* result=nullptr);
    static QVariantList fromModel(const QAbstractItemModel* model, bool* result=nullptr);

private:
    template<typename T>
    bool setStorageImpl(T&& storage);

    template<typename T>
    bool replaceStorageImpl(T&& storage);

    template<typename T>
    bool patchStorageImpl(T&& storage);

    bool m_completed=false;
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;
    QHash<QString, int> m_roleIds;

    QVariantList m_storage;
    QVariantList m_backup;
    QSPatchSet m_patches;

    QTimer m_selectInhibitTimer;
    QTimer m_submitInhibitTimer;
    QTimer m_revertInhibitTimer;
    QTimer m_updateInhibitTimer;

    QElapsedTimer m_selectTimer;
    QElapsedTimer m_submitTimer;
    QElapsedTimer m_revertTimer;
    QElapsedTimer m_updateTimer;

    // ──────── ITERATOR ──────────
public:
    using iterator = typename QVariantList::iterator;
    using const_iterator = typename QVariantList::const_iterator;
    iterator begin() { return m_storage.begin(); }
    iterator end() { return m_storage.end(); }
    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }
    const_iterator cbegin() const { return m_storage.cbegin(); }
    const_iterator cend() const { return m_storage.cend(); }
    const_iterator constBegin() const { return m_storage.constBegin(); }
    const_iterator constEnd() const { return m_storage.constEnd(); }

    using reverse_iterator = typename QVariantList::const_reverse_iterator;
    using const_reverse_iterator = typename QVariantList::const_reverse_iterator;
    reverse_iterator rbegin() { return m_storage.rbegin(); }
    reverse_iterator rend() { return m_storage.rend(); }
    const_reverse_iterator rbegin() const { return m_storage.rbegin(); }
    const_reverse_iterator rend() const { return m_storage.rend(); }
    const_reverse_iterator crbegin() const { return m_storage.crbegin(); }
    const_reverse_iterator crend() const { return m_storage.crend(); }
};

#endif // QVARIANTLISTMODEL_H
