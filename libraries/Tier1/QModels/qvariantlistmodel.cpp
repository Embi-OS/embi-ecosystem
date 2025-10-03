#include "qvariantlistmodel.h"
#include "qmodels_log.h"

#include "syncable/qsdiffrunner.h"

// ──────── CONSTRUCTOR ──────────
QVariantListModel::QVariantListModel(QObject *parent) :
    QVariantListModel("", parent)
{

}

QVariantListModel::QVariantListModel(const QString& baseName, QObject * parent) :
    QAbstractListModel(parent),
    m_baseName(baseName),
    m_syncable(false),
    m_selectInhibitTimer(this),
    m_submitInhibitTimer(this),
    m_revertInhibitTimer(this),
    m_updateInhibitTimer(this)
{
    m_selectInhibitTimer.setInterval(0);
    m_selectInhibitTimer.setSingleShot(true);
    connect(&m_selectInhibitTimer, &QTimer::timeout, this, &QVariantListModel::select, Qt::UniqueConnection);
    connect(this, &QVariantListModel::selectInhibitTimeChanged, this, &QVariantListModel::onSelectInhibitTimeChanged);

    m_submitInhibitTimer.setInterval(0);
    m_submitInhibitTimer.setSingleShot(true);
    connect(&m_submitInhibitTimer, &QTimer::timeout, this, &QVariantListModel::submit, Qt::UniqueConnection);
    connect(this, &QVariantListModel::submitInhibitTimeChanged, this, &QVariantListModel::onSubmitInhibitTimeChanged);

    m_revertInhibitTimer.setInterval(0);
    m_revertInhibitTimer.setSingleShot(true);
    connect(&m_revertInhibitTimer, &QTimer::timeout, this, &QVariantListModel::revert, Qt::UniqueConnection);
    connect(this, &QVariantListModel::revertInhibitTimeChanged, this, &QVariantListModel::onRevertInhibitTimeChanged);

    m_updateInhibitTimer.setInterval(0);
    m_updateInhibitTimer.setSingleShot(true);
    connect(&m_updateInhibitTimer, &QTimer::timeout, this, &QVariantListModel::update, Qt::UniqueConnection);
    connect(this, &QVariantListModel::updateInhibitTimeChanged, this, &QVariantListModel::onUpdateInhibitTimeChanged);

    connect(this, &QAbstractItemModel::rowsInserted, this, &QVariantListModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QVariantListModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QVariantListModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QVariantListModel::countInvalidate);

    connect(this, &QAbstractItemModel::dataChanged, this, &QVariantListModel::contentInvalidate);
    connect(this, &QAbstractItemModel::rowsInserted, this, &QVariantListModel::contentInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QVariantListModel::contentInvalidate);
    connect(this, &QAbstractItemModel::rowsMoved, this, &QVariantListModel::contentInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QVariantListModel::contentInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QVariantListModel::contentInvalidate);

    connect(this, &QVariantListModel::rolesChanged, this, [this](const QStringList& roles){
        QVariantMap map;
        for(const QString& field: roles)
            map.insert(field, QVariant());
        updateRoleNames(map);
    });
    connect(this, &QVariantListModel::placeholderChanged, this, [this](const QVariantMap& placeholder){
        updateRoleNames(placeholder);
    });

    connect(this, &QVariantListModel::baseNameChanged, this, &QVariantListModel::queueSelect);
    connect(this, &QVariantListModel::sourceChanged, this, &QVariantListModel::queueSelect);

    connect(this, &QVariantListModel::selectWhenChanged, this, &QVariantListModel::queueSelect);
    connect(this, &QVariantListModel::submitWhenChanged, this, &QVariantListModel::queueSubmit);
    connect(this, &QVariantListModel::revertWhenChanged, this, &QVariantListModel::queueRevert);
}

QVariantListModel::~QVariantListModel()
{
    m_storage.clear();
    m_storage.squeeze();
    m_backup.clear();
    m_backup.squeeze();
}

void QVariantListModel::classBegin()
{

}

void QVariantListModel::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    if(m_selectPolicy<QVariantListModelPolicies::Postponed && m_selectWhen)
        select();
}

bool QVariantListModel::isCompleted() const
{
    return m_completed;
}

const QModelIndex& QVariantListModel::noParent()
{
    static const QModelIndex ret = QModelIndex();
    return ret;
}

// ──────── ABSTRACT MODEL OVERRIDE ──────────
QVariant QVariantListModel::data(const QModelIndex & index, const int role) const
{
    if(!index.isValid() || index.row() >= m_storage.count())
        return QVariant();

    if(role == Qt::UserRole)
    {
        return m_storage.at(index.row());
    }
    else if(role > Qt::UserRole)
    {
        if(m_storage.at(index.row()).metaType().id()==QMetaType::QVariantMap)
        {
            const QVariantMap& original = *reinterpret_cast<const QVariantMap*>(m_storage[index.row()].constData());
            const QString& key = m_roleNames.value(role);
            return original.value(key, m_placeholder.value(key));
        }
    }

    return QVariant();
}

bool QVariantListModel::setData(const QModelIndex & index, const QVariant & value, const int role)
{
    if(!index.isValid() || index.row() >= m_storage.count())
        return false;

    bool ret=false;
    if(role == Qt::UserRole)
    {
        m_storage.replace(index.row(), value);
        emit this->dataChanged (index, index, QVector<int>{});
        ret = true;
    }
    else if(role > Qt::UserRole)
    {
        if(m_storage.at(index.row()).metaType().id()==QMetaType::QVariantMap)
        {
            QVariantMap& original = *reinterpret_cast<QVariantMap*>(m_storage[index.row()].data());
            const QString& key = m_roleNames.value(role);
            const QVariant& oldValue = original[key];
            if(oldValue==value && value.isNull()==oldValue.isNull())
                return true;
            original[key] = value;
            emit this->dataChanged (index, index, QVector<int>{role});
            ret = true;
        }
    }

    if(!ret) {
        QMODELSLOG_WARNING()<<m_baseName<<"cannot setData to:"<<index<<role;
    }

    return ret;
}

QHash<int, QByteArray> QVariantListModel::roleNames (void) const
{
    return m_roleNames;
}

int QVariantListModel::rowCount (const QModelIndex & parent) const
{
    return (!parent.isValid() ? m_storage.count() : 0);
}

void QVariantListModel::shuffle()
{
    beginResetModel();

    shuffle(m_storage);

    endResetModel();
}

#include <random>
void QVariantListModel::shuffle(QVariantList& storage)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(storage.begin(), storage.end(), g);
}

void QVariantListModel::sort(int role, Qt::SortOrder order)
{
    const QString& roleName = m_roleNames[role];
    sort(m_storage, roleName, order);
}

void QVariantListModel::sort(const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    beginResetModel();

    sort(m_storage, roleName, order, cs, isLocaleAware);

    endResetModel();
}

void QVariantListModel::sort(QVariantList& storage, const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    auto comp = [&](const QVariant& left, const QVariant& right) -> bool {
        const QVariantMap& leftMap = *reinterpret_cast<const QVariantMap*>(left.constData());
        const QVariantMap& rightMap = *reinterpret_cast<const QVariantMap*>(right.constData());

        return order==Qt::AscendingOrder ?
                   qVariantLessThan(leftMap[roleName], rightMap[roleName], cs, isLocaleAware) :
                   qVariantLessThan(rightMap[roleName], leftMap[roleName], cs, isLocaleAware);
    };

    std::stable_sort(storage.begin(),storage.end(), comp);
}

int QVariantListModel::indexOf(int role, const QVariant& val, bool isSorted, Qt::SortOrder order, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    const QString& roleName = m_roleNames[role];
    return indexOf(roleName, val, isSorted, order, upperBound, cs, isLocaleAware);
}

int QVariantListModel::indexOf(const QString& roleName, const QVariant& val, bool isSorted, Qt::SortOrder order, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return indexOf(m_storage, roleName, val, isSorted, order, upperBound, cs, isLocaleAware);
}

int QVariantListModel::indexOf(const QVariantList& storage, const QString &roleName, const QVariant& val, bool isSorted, Qt::SortOrder order, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (storage.isEmpty())
        return -1;

    auto comp = [&](const QVariant& left, const QVariant& right) -> bool {
        const QVariantMap& leftMap = *reinterpret_cast<const QVariantMap*>(left.constData());
        const QVariantMap& rightMap = *reinterpret_cast<const QVariantMap*>(right.constData());

        return order==Qt::AscendingOrder ?
                   qVariantLessThan(leftMap[roleName], rightMap[roleName], cs, isLocaleAware) :
                   qVariantLessThan(rightMap[roleName], leftMap[roleName], cs, isLocaleAware);
    };

    if (isSorted) {
        const QVariant searchItem = QVariantMap{{roleName, val}};
        auto it = upperBound
                      ? std::upper_bound(storage.begin(), storage.end(), searchItem, comp)
                      : std::lower_bound(storage.begin(), storage.end(), searchItem, comp);

        if (it != storage.end()) {
            const QVariantMap& foundMap = *reinterpret_cast<const QVariantMap*>(it->constData());
            const QVariant& foundValue = foundMap[roleName];

            if ((upperBound && comp(searchItem, foundMap)) || // upperBound => strictement supérieur ou inférieur selon order
                (!upperBound && qVariantCompare(foundValue, val, cs, isLocaleAware) == 0)) { // lowerBound => égalité requise
                return std::distance(storage.begin(), it);
            }
        }
    } else {
        auto it = std::find_if(storage.begin(), storage.end(), [&](const QVariant& item) {
            const QVariantMap& itemMap = *reinterpret_cast<const QVariantMap*>(item.constData());
            return qVariantCompare(itemMap[roleName], val, cs, isLocaleAware)==0;
        });

        if (it != storage.end()) {
            return std::distance(storage.begin(), it);
        }
    }

    // Non trouvé
    return upperBound ? storage.size() : -1;
}

// ──────── PUBLIC API ──────────

bool QVariantListModel::append(const QJSValue& value)
{
    return insert(count(), value);
}

bool QVariantListModel::prepend(const QJSValue& value)
{
    return insert(0, value);
}

bool QVariantListModel::insert(int index, const QJSValue& value)
{
    return insert(index, value.toVariant());
}

bool QVariantListModel::append(const QVariant& variant)
{
    return insert(count(), variant);
}

bool QVariantListModel::prepend(const QVariant& variant)
{
    return insert(0, variant);
}

bool QVariantListModel::insert(int index, const QVariant& variant)
{
    if(index > count())
    {
        QMODELSLOG_DEBUG()<<"index "<<index<<"is greater than count"<<count()
                  <<"The item will be inserted at the end of the list";
        index = count();
    }
    else if(index < 0)
    {
        QMODELSLOG_DEBUG()<<"index "<<index<<"is lower than 0"
                  <<"The item will be inserted at the beginning of the list";
        index = 0;
    }

    if(variant.metaType().id() == QMetaType::QVariantList)
    {
        const QVariantList variants = variant.toList();
        if(variants.isEmpty())
            return true;

        updateRoleNames(variants.first());

        beginInsertRows(noParent(), index, index + variants.count() - 1);
        m_storage.reserve(m_storage.count() + variants.count());
        int offset = 0;
        for(const QVariant& var: variants)
        {
            const int idx = index+offset;
            m_storage.insert(idx, var);
            offset++;
        }
        endInsertRows();
    }
    else
    {
        updateRoleNames(variant);
        beginInsertRows(noParent(), index, index);
        m_storage.insert(index, variant);
        endInsertRows();
    }

    return true;
}

QVariant QVariantListModel::at(const int index) const
{
    return get(index);
}

QVariant QVariantListModel::get(const int index) const
{
    if(index < 0 || index >= m_storage.size())
    {
        QMODELSLOG_WARNING()<<"The index"<<index<<"is out of bound.";
        return QVariant();
    }
    return m_storage.at(index);
}

bool QVariantListModel::contains(const QVariant& variant) const
{
    return m_storage.contains(variant);
}

bool QVariantListModel::set(int index, const QVariant& variant)
{
    if (index < 0 || index > m_storage.size())
    {
        QMODELSLOG_WARNING()<<"The index"<<index<<"is out of bound.";
        return false;
    }

    if (index == m_storage.size())
        return append(variant);

    if(m_storage[index]==variant)
        return false;

    if(m_storage.at(index).metaType().id()==QMetaType::QVariantMap &&
       variant.metaType().id()==QMetaType::QVariantMap)
    {
        QVariantMap& original = *reinterpret_cast<QVariantMap*>(m_storage[index].data());
        const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(variant.constData());

        QVector<int> roles;
        for (auto [key, value] : map.asKeyValueRange())
        {
            const QVariant& oldValue = original[key];
            if(oldValue==value || !m_roleIds.contains(key))
                continue;
            original[key] = value;
            roles << m_roleIds[key];
        }

        if(!roles.isEmpty())
            emit this->dataChanged(this->index(index,0), this->index(index,0), roles);
    }
    else
    {
        m_storage.replace(index, variant);
        emit this->dataChanged(this->index(index,0), this->index(index,0));
    }

    return true;
}

bool QVariantListModel::move(int from, int to, int count)
{
    if(from < 0 || (from + count - 1) >= m_storage.size())
    {
        QMODELSLOG_DEBUG()<<"Can't move an object whose index is out of bound";
        return false;
    }

    if(to < 0 || (to + count - 1) >= m_storage.size())
    {
        QMODELSLOG_DEBUG()<<"Can't move an object to a position that is out of bound";
        return false;
    }

    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to > from ? to + count : to);
    for(int i = 0; i < count; ++i)
    {
        m_storage.move(from, to);
    }
    endMoveRows();

    return true;
}

bool QVariantListModel::moveUp(int index)
{
    // Move index to index-1
    if(index <= 0 || index >= count())
    {
        QMODELSLOG_DEBUG()<<"The index is the first of the list or index is out of bound";
        return false;
    }

    return move(index, index - 1);
}

bool QVariantListModel::moveDown(int index)
{
    // Move index to index+1
    if(!(count() &&  // There is a least one entry
          index >= 0 &&  // We can be from the first
          index < (count() - 1))  // To the last one minus 1
        )
    {
        QMODELSLOG_DEBUG()<<"The index is the last of the list or index is out of bound";
        return false;
    }

    return move(index, index + 1);
}

bool QVariantListModel::remove(int index, int count)
{
    if(index < 0 || (index + count - 1) >= m_storage.size())
    {
        QMODELSLOG_WARNING()<<"Can't remove an object whose index is out of bound";
        return false;
    }

    beginRemoveRows(noParent(), index, index + count - 1);
    m_storage.remove(index, count);
    endRemoveRows();

    return true;
}

bool QVariantListModel::remove(QList<int> indexes)
{
    bool ret=true;

    std::stable_sort(indexes.rbegin(), indexes.rend());

    for(int index: indexes)
    {
        if(!remove(index))
            ret = false;
    }
    return ret;
}

bool QVariantListModel::clear()
{
    if(m_storage.isEmpty())
        return true;

    beginResetModel();
    m_storage.clear();
    m_storage.squeeze();
    m_backup.clear();
    m_backup.squeeze();
    endResetModel();

    return true;
}

void QVariantListModel::alter(const QSPatch &patch)
{
    if(patch.isNull())
        return;

    m_patches.append(patch);
    queueUpdate();
}

int QVariantListModel::roleForName(const QString& name) const
{
    return m_roleIds.value(name, -1);
}
QByteArray QVariantListModel::roleName(int role) const
{
    return m_roleNames.value(role, QByteArray());
}

const QVariantList& QVariantListModel::getStorage() const
{
    return m_storage;
}

const QVariantList& QVariantListModel::getBackup() const
{
    return m_backup;
}

bool QVariantListModel::setStorage(const QVariantList& storage)
{
    return setStorageImpl(storage);
}

bool QVariantListModel::setStorage(QVariantList&& storage)
{
    return setStorageImpl(std::move(storage));
}

// Surcharges internes
bool QVariantListModel::replaceStorage(const QVariantList& storage)
{
    return replaceStorageImpl(storage);
}

bool QVariantListModel::replaceStorage(QVariantList&& storage)
{
    return replaceStorageImpl(std::move(storage));
}

bool QVariantListModel::patchStorage(const QVariantList& storage)
{
    return patchStorageImpl(storage);
}

bool QVariantListModel::patchStorage(QVariantList&& storage)
{
    return patchStorageImpl(std::move(storage));
}

// Implémentations templatisées

template bool QVariantListModel::setStorageImpl<const QVariantList&>(const QVariantList&);
template bool QVariantListModel::setStorageImpl<QVariantList&&>(QVariantList&&);
template bool QVariantListModel::replaceStorageImpl<const QVariantList&>(const QVariantList&);
template bool QVariantListModel::replaceStorageImpl<QVariantList&&>(QVariantList&&);
template bool QVariantListModel::patchStorageImpl<const QVariantList&>(const QVariantList&);
template bool QVariantListModel::patchStorageImpl<QVariantList&&>(QVariantList&&);

template<typename T>
bool QVariantListModel::setStorageImpl(T&& storage)
{
    bool result = false;

    if (m_syncable)
        result = patchStorage(std::forward<T>(storage));
    else
        result = replaceStorage(std::forward<T>(storage));

    m_backup.clear();
    m_backup.squeeze();

    if (m_backupable)
        m_backup = m_storage;

    emit this->storageChanged(m_storage);

    return result;
}

template<typename T>
bool QVariantListModel::replaceStorageImpl(T&& storage)
{
    if (m_storage.isEmpty() && storage.isEmpty())
        return true;

    if (!storage.isEmpty())
        updateRoleNames(storage.first());

    QElapsedTimer timer;
    timer.start();

    beginResetModel();
    m_storage.clear();
    m_storage.squeeze();
    m_storage = std::forward<T>(storage);
    endResetModel();

    QMODELSLOG_TRACE()<<this<<m_baseName<<"replaceStorage took:"<<timer.nsecsElapsed()/1000000.0<<"ms";

    return true;
}

template<typename T>
bool QVariantListModel::patchStorageImpl(T&& storage)
{
    if (m_storage.isEmpty() && storage.isEmpty())
        return true;

    if (m_roleNames.isEmpty() || m_storage.isEmpty())
        return replaceStorage(std::forward<T>(storage));

    QElapsedTimer timer;
    timer.start();

    const QSPatchSet patches = QSDiffRunner::compare(m_storage, storage, m_primaryField);
    bool result = QSDiffRunner::patch(this, patches);
    m_storage.squeeze();

    QMODELSLOG_TRACE()<<this<<m_baseName<<"patchStorage took:"<<timer.nsecsElapsed()/1000000.0<<"ms";

    return result;
}

void QVariantListModel::queueSelect()
{
    if(!m_completed || !m_selectWhen)
        return;

    if (m_selectPolicy==QVariantListModelPolicies::Direct) {
        select();
    }
    else if (m_selectPolicy==QVariantListModelPolicies::Delayed) {
        if(!m_selectInhibitTimer.isActive()) {
            setSelectQueued(true);
            m_selectInhibitTimer.start();
        }
    }
    else if(m_selectPolicy==QVariantListModelPolicies::Postponed) {
        if(m_selectPostponed && !m_selectInhibitTimer.isActive()) {
            setSelectQueued(true);
            m_selectInhibitTimer.start();
        }
    }
    else if (m_selectPolicy==QVariantListModelPolicies::Manual) {
        setSelectQueued(true);
    }
}

bool QVariantListModel::select()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    setSelectPostponed(true);
    setSelectQueued(false);
    m_selectInhibitTimer.stop();

    if(!m_selectWhen)
        return false;

    if (m_selectPolicy==QVariantListModelPolicies::Disabled) {
        QMODELSLOG_WARNING()<<this<<"Select policy is disabled for"<<m_baseName;
        return false;
    }

    m_selectTimer.restart();

    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }

    setSelecting(true);
    setLoading(true);

    bool result = doSelect();

    return result;
}

bool QVariantListModel::waitForSelect(int timeout)
{
    if(!m_selecting && !m_selectQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantListModel::selectDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantListModel::doSelect()
{
    bool result = true;
    QVariantList storage = selectSource(&result);
    setStorage(std::move(storage));
    emitSelectDone(true);

    return true;
}

void QVariantListModel::queueSubmit()
{
    if(!m_completed || !m_submitWhen)
        return;

    if (m_submitPolicy==QVariantListModelPolicies::Direct) {
        submit();
    }
    else if (m_submitPolicy==QVariantListModelPolicies::Delayed) {
        if(!m_submitInhibitTimer.isActive()) {
            setSubmitQueued(true);
            m_submitInhibitTimer.start();
        }
    }
    else if(m_submitPolicy==QVariantListModelPolicies::Postponed) {
        if(m_submitPostponed && !m_submitInhibitTimer.isActive()) {
            setSubmitQueued(true);
            m_submitInhibitTimer.start();
        }
    }
    else if (m_submitPolicy==QVariantListModelPolicies::Manual) {
        setSubmitQueued(true);
    }
}

bool QVariantListModel::submit()
{
    setSubmitPostponed(true);
    setSubmitQueued(false);
    m_submitInhibitTimer.stop();

    if(!m_submitWhen)
        return false;

    if (m_submitPolicy==QVariantListModelPolicies::Disabled) {
        QMODELSLOG_WARNING()<<this<<"Submit policy is disabled for"<<m_baseName;
        return false;
    }

    if(!m_isDirty)
        return true;

    m_submitTimer.restart();

    setSubmiting(true);
    setLoading(true);

    // TODO: generate dirtyKeys
    bool result = doSubmit();

    return result;
}

bool QVariantListModel::waitForSubmit(int timeout)
{
    if(!m_submiting && !m_submitQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantListModel::submitDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantListModel::doSubmit()
{
    emitSubmitDone(true);

    return true;
}

void QVariantListModel::queueRevert()
{
    if(!m_completed || !m_revertWhen)
        return;

    if (m_revertPolicy==QVariantListModelPolicies::Direct) {
        revert();
    }
    else if (m_revertPolicy==QVariantListModelPolicies::Delayed) {
        if(!m_revertInhibitTimer.isActive()) {
            setRevertQueued(true);
            m_revertInhibitTimer.start();
        }
    }
    else if(m_revertPolicy==QVariantListModelPolicies::Postponed) {
        if(m_revertPostponed && !m_revertInhibitTimer.isActive()) {
            setRevertQueued(true);
            m_revertInhibitTimer.start();
        }
    }
    else if (m_revertPolicy==QVariantListModelPolicies::Manual) {
        setRevertQueued(true);
    }
}

void QVariantListModel::revert()
{
    setRevertPostponed(true);
    setRevertQueued(false);
    m_revertInhibitTimer.stop();

    if(!m_revertWhen)
        return;

    if (m_revertPolicy==QVariantListModelPolicies::Disabled) {
        QMODELSLOG_WARNING()<<this<<"Revert policy is disabled for"<<m_baseName;
        return;
    }

    m_revertTimer.restart();

    setReverting(true);
    setLoading(true);

    bool result = doRevert();

    Q_UNUSED(result)
}

bool QVariantListModel::waitForRevert(int timeout)
{
    if(!m_reverting && !m_revertQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantListModel::revertDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantListModel::doRevert()
{
    setStorage(m_backup);
    emitRevertDone(true);

    return true;
}

void QVariantListModel::queueUpdate()
{
    if(!m_completed || !m_updateWhen)
        return;

    if (m_updatePolicy==QVariantListModelPolicies::Direct) {
        update();
    }
    else if (m_updatePolicy==QVariantListModelPolicies::Delayed) {
        if(!m_updateInhibitTimer.isActive()) {
            setUpdateQueued(true);
            m_updateInhibitTimer.start();
        }
    }
    else if(m_updatePolicy==QVariantListModelPolicies::Postponed) {
        if(m_updatePostponed && !m_updateInhibitTimer.isActive()) {
            setUpdateQueued(true);
            m_updateInhibitTimer.start();
        }
    }
    else if (m_updatePolicy==QVariantListModelPolicies::Manual) {
        setUpdateQueued(true);
    }
}

void QVariantListModel::update()
{
    setUpdatePostponed(true);
    setUpdateQueued(false);
    m_updateInhibitTimer.stop();

    if(!m_updateWhen)
        return;

    if (m_updatePolicy==QVariantListModelPolicies::Disabled) {
        QMODELSLOG_WARNING()<<this<<"Update policy is disabled for"<<m_baseName;
        return;
    }

    m_updateTimer.restart();

    if(m_submitPolicy<QVariantListModelPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }

    setUpdating(true);
    setLoading(true);

    bool result = doUpdate();

    Q_UNUSED(result)
}

bool QVariantListModel::waitForUpdate(int timeout)
{
    if(!m_updating && !m_updateQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantListModel::updateDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantListModel::doUpdate()
{
    if(m_patches.isEmpty()) {
        emitUpdateDone(true);
        return true;
    }

    const QSPatchSet patches = QSDiffRunner::merge(m_patches);
    bool result = QSDiffRunner::patch(this, patches);
    m_patches.clear();

    emitUpdateDone(true);

    return result;
}

bool QVariantListModel::cancel()
{
    return doCancel();
}

bool QVariantListModel::doCancel()
{
    // Cancel current processing if any
    return false;
}

QVariantList QVariantListModel::selectSource(bool* result)
{
    if(!m_source.isValid())
    {
        if(result)
            *result=false;
        return QVariantList();
    }

    if(m_source.canConvert<QVariantList>())
    {
        if(result)
            *result=true;
        return m_source.toList();
    }
    else if(m_source.canConvert<QAbstractItemModel*>())
    {
        const QAbstractItemModel* model = m_source.value<QAbstractItemModel*>();
        return fromModel(model, result);
    }

    QMODELSLOG_WARNING()<<m_baseName<<"Unable to load source:"<<m_source;

    if(result)
        *result=true;

    return QVariantList();
}

QVariantList QVariantListModel::fromModel(const QAbstractItemModel* model, bool* result)
{
    if(!model)
    {
        if(result)
            *result=false;
        return QVariantList();
    }

    if(const QVariantListModel* varModel = qobject_cast<const QVariantListModel*>(model))
    {
        if(result)
            *result=true;
        return varModel->getStorage();
    }

    QVariantList variants;
    variants.reserve(model->rowCount());

    const QHash<int,QByteArray> names = model->roleNames();
    for(int i=0; i<model->rowCount(); ++i)
    {
        QVariantMap map;
        const QModelIndex modelIndex = model->index(i, 0);
        for(auto [key, value]: names.asKeyValueRange())
            map.insert(value, model->data(modelIndex, key));
        variants.append(map);
    }

    if(result)
        *result=true;

    return variants;
}

void QVariantListModel::emitSelectDone(bool result)
{
    setSelected(true);

    setIsDirty(false);
    setSelecting(false);
    setLoading(false);

    QMODELSLOG_DEBUG()<<this<<m_baseName<<"select took:"<<m_selectTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->selectDone(result);
}

void QVariantListModel::emitSubmitDone(bool result)
{
    if (m_backupable)
        m_backup = m_storage;
    else
        m_backup.clear();
    setIsDirty(false);
    setSubmiting(false);
    setLoading(false);

    QMODELSLOG_DEBUG()<<this<<m_baseName<<"submit took:"<<m_submitTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->submitDone(result);
}

void QVariantListModel::emitRevertDone(bool result)
{
    setIsDirty(false);
    setReverting(false);
    setLoading(false);

    QMODELSLOG_DEBUG()<<this<<m_baseName<<"revert took:"<<m_revertTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->revertDone(result);
}

void QVariantListModel::emitUpdateDone(bool result)
{
    if (m_backupable)
        m_backup = m_storage;
    else
        m_backup.clear();
    setIsDirty(false);
    setUpdating(false);
    setLoading(false);

    QMODELSLOG_DEBUG()<<this<<m_baseName<<"update took:"<<m_updateTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->updateDone(result);
}

void QVariantListModel::onSelectInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_selectInhibitTimer.isActive();
    m_selectInhibitTimer.stop();
    m_selectInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_selectInhibitTimer.start();
}

void QVariantListModel::onSubmitInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_submitInhibitTimer.isActive();
    m_submitInhibitTimer.stop();
    m_submitInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_submitInhibitTimer.start();
}

void QVariantListModel::onRevertInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_submitInhibitTimer.isActive();
    m_revertInhibitTimer.stop();
    m_revertInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_revertInhibitTimer.start();
}

void QVariantListModel::onUpdateInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_updateInhibitTimer.isActive();
    m_updateInhibitTimer.stop();
    m_updateInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_updateInhibitTimer.start();
}

// ──────── ABSTRACT MODEL PRIVATE ──────────
void QVariantListModel::countInvalidate()
{
    const int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}

void QVariantListModel::contentInvalidate()
{
    if(m_selecting || m_submiting || m_reverting || m_updating)
        return;

    setIsDirty(true);

    emit this->storageChanged(m_storage);
    queueSubmit();
}

void QVariantListModel::clearRoleNames()
{
    m_roleNames.clear();
    m_roleIds.clear();
}

void QVariantListModel::updateRoleNames(const QVariant& var)
{
    if(m_roleNames.isEmpty())
    {
        QHash<int, QByteArray> roleNames;
        if(var.canConvert<QVariantMap>())
        {
            const QVariantMap& firstElement = var.toMap();
            QMapIterator<QString,QVariant> iter(firstElement);
            int role = Qt::UserRole+1;

            roleNames.reserve(firstElement.size());
            while (iter.hasNext())
            {
                iter.next();
                roleNames[role] = iter.key().toLocal8Bit();
                role++;
            }
        }
        else
        {
            roleNames[Qt::UserRole] = QByteArrayLiteral("qtVariant");
        }

        m_roleNames = roleNames;
        reverseStringIntHash(m_roleIds, m_roleNames);
    }
}
