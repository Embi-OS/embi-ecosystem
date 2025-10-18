#include "qvariantmapper.h"
#include "qutils_log.h"

QVariantMapper::QVariantMapper(QObject *parent) :
    QVariantMapper("", parent)
{

}

QVariantMapper::QVariantMapper(const QString& baseName, QObject *parent, const QMetaObject* blacklistedMetaObject) :
    QObject(parent),
    m_baseName(baseName),
    m_blacklistedMetaObject(blacklistedMetaObject?blacklistedMetaObject:&QVariantMapper::staticMetaObject),
    m_selectInhibitTimer(this),
    m_submitInhibitTimer(this),
    m_revertInhibitTimer(this),
    m_updateInhibitTimer(this)
{
    Q_ASSERT_X(m_blacklistedMetaObject && m_blacklistedMetaObject->inherits(&QVariantMapper::staticMetaObject), "QVariantMapper::initProperties", "blacklistedMetaObject cannot return type that does not inherits from QVariantMapper");

    m_selectInhibitTimer.setInterval(0);
    m_selectInhibitTimer.setSingleShot(true);
    connect(&m_selectInhibitTimer, &QTimer::timeout, this, &QVariantMapper::select, Qt::UniqueConnection);
    connect(this, &QVariantMapper::selectInhibitTimeChanged, this, &QVariantMapper::onSelectInhibitTimeChanged);

    m_submitInhibitTimer.setInterval(0);
    m_submitInhibitTimer.setSingleShot(true);
    connect(&m_submitInhibitTimer, &QTimer::timeout, this, &QVariantMapper::submit, Qt::UniqueConnection);
    connect(this, &QVariantMapper::submitInhibitTimeChanged, this, &QVariantMapper::onSubmitInhibitTimeChanged);

    m_revertInhibitTimer.setInterval(0);
    m_revertInhibitTimer.setSingleShot(true);
    connect(&m_revertInhibitTimer, &QTimer::timeout, this, &QVariantMapper::revert, Qt::UniqueConnection);
    connect(this, &QVariantMapper::revertInhibitTimeChanged, this, &QVariantMapper::onRevertInhibitTimeChanged);

    m_updateInhibitTimer.setInterval(0);
    m_updateInhibitTimer.setSingleShot(true);
    connect(&m_updateInhibitTimer, &QTimer::timeout, this, &QVariantMapper::update, Qt::UniqueConnection);
    connect(this, &QVariantMapper::updateInhibitTimeChanged, this, &QVariantMapper::onUpdateInhibitTimeChanged);

    connect(this, &QVariantMapper::valueChanged, this, &QVariantMapper::contentInvalidate);

    connect(this, &QVariantMapper::baseNameChanged, this, &QVariantMapper::queueSelect);

    connect(this, &QVariantMapper::selectWhenChanged, this, &QVariantMapper::queueSelect);
    connect(this, &QVariantMapper::submitWhenChanged, this, &QVariantMapper::queueSubmit);
    connect(this, &QVariantMapper::revertWhenChanged, this, &QVariantMapper::queueRevert);
    connect(this, &QVariantMapper::updateWhenChanged, this, &QVariantMapper::queueUpdate);
}

QVariantMapper::~QVariantMapper()
{

}

void QVariantMapper::classBegin()
{

}

void QVariantMapper::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    if(m_selectPolicy<QVariantMapperPolicies::Postponed && m_selectWhen)
        select();
}

bool QVariantMapper::isCompleted() const
{
    return m_completed;
}

QVariant QVariantMapper::value(const QString &key, const QVariant &defaultValue) const
{
    return m_storage.value(key, m_placeholder.value(key, defaultValue));
}

bool QVariantMapper::setValue(const QString &key, const QVariant &value)
{
    if(!m_mappedKeys.contains(key) && !m_mappedKeys.isEmpty())
        return false;
    if(m_storage.contains(key) && m_storage.value(key,value)==value)
        return false;
    m_storage.insert(key,value);
    emit this->valueChanged(key, value);
    return true;
}

bool QVariantMapper::reset()
{
    if(m_storage.isEmpty())
        return false;
    const QStringList keys = m_storage.keys();
    bool result = true;
    for(const QString& key: keys)
        result &= setValue(key, QVariant());
    return result;
}

bool QVariantMapper::clear()
{
    if(m_storage.isEmpty())
        return false;
    m_storage.clear();
    contentInvalidate();
    return true;
}

QVariantMap QVariantMapper::values(QStringList keys) const
{
    if(keys.isEmpty())
        keys = this->keys();

    QVariantMap map;
    for(const QString& key: std::as_const(keys))
        map.insert(key, value(key));

    return map;
}

void QVariantMapper::remove(const QString &key)
{
    m_storage.remove(key);
}

bool QVariantMapper::contains(const QString &key)
{
    return m_storage.contains(key);
}

QStringList QVariantMapper::keys() const
{
    QStringList keys = m_storage.keys();

    const QStringList placeholderKeys = m_placeholder.keys();
    for(const QString& key: placeholderKeys) {
        if(!keys.contains(key))
            keys.append(key);
    }

    keys.sort(Qt::CaseInsensitive);

    return keys;
}

QStringList QVariantMapper::keys(const QVariant &value) const
{
    return m_storage.keys(value);
}

void QVariantMapper::mapObject(QObject* object)
{
    if(!object)
        return;

    const QMetaObject smo = QObject::staticMetaObject;
    const QMetaObject* mo = object->metaObject();
    const int sCount = smo.propertyCount();
    const int count = mo->propertyCount();

    // We must iterate in reverse so that properties are stored in order in m_mappedProperties
    for(int i=count; i<sCount; i--)
    {
        QMetaProperty mp = mo->property(i);
        const QString &propertyName = mp.name();
        mapProperty(object, propertyName);
    }
}

bool QVariantMapper::mapProperty(QObject* object, const QString& property, const QString& aliasName)
{
    if(!object)
        return false;

    const QMetaObject *mo = object->metaObject();
    const int index = mo->indexOfProperty(property.toUtf8());

    if(index<0)
        return false;

    const QMetaProperty mp = mo->property(index);
    const QString key = aliasName.isEmpty()?property:aliasName;
    MappedProperty mappedProperty = MappedProperty(key, mp);
    QList<MappedProperty> mappedProperties = m_mappedProperties.values(object);
    if(mappedProperties.contains(mappedProperty))
        return true;

    m_mappedProperties.insert(object, MappedProperty(key, mp));
    if(!m_mappedKeys.contains(key))
        m_mappedKeys.append(key);

    const QVariant previousValue = readProperty(object, mp);
    const QVariant currentValue = this->value(key, previousValue);
    if (this->contains(key) &&
        (!previousValue.isValid() || previousValue != currentValue))
    {
        writeProperty(object, mp, currentValue);
    }

    if (!contains(key))
    {
        m_storage.insert(key,previousValue);
        m_backup.insert(key,previousValue);
        m_cache.insert(key,previousValue);
        if(!m_placeholder.contains(key))
            m_placeholder.insert(key,previousValue);
    }

    if (mp.hasNotifySignal())
    {
        static const char* HANDLER = "onPropertyChanged()";
        const QMetaMethod notifySignal = mp.notifySignal();
        const QMetaMethod updateSlot = metaObject()->method(metaObject()->indexOfSlot(HANDLER));
        connect(object,notifySignal,this,updateSlot);
    }

    QUTILSLOG_TRACE()<<"New property mapped to:"<<this<<object<<key;

    return true;
}

void QVariantMapper::clearProperties()
{
    QMetaMethod updateSlot = metaObject()->method(metaObject()->indexOfSlot("onPropertyChanged()"));

    QMultiHashIterator<QObject*, MappedProperty> i(m_mappedProperties);
    while(i.hasNext())
    {
        i.next();
        QObject* object = i.key();
        const MappedProperty mappedProperty = i.value();
        const QMetaProperty mp = mappedProperty.property;

        if(!mp.hasNotifySignal())
            continue;

        const QMetaMethod notifySignal = mp.notifySignal();
        QObject::disconnect(object, notifySignal, this, updateSlot);
    }
    m_mappedProperties.clear();
}

void QVariantMapper::initProperties()
{
    const QMetaObject* smo = m_blacklistedMetaObject;
    const QMetaObject* mo = metaObject();
    const int sCount = smo->propertyCount();
    const int count = mo->propertyCount();

    // We must iterate in reverse so that properties are stored in order in m_mappedProperties
    for(int i=count; i>=sCount; i--)
    {
        QMetaProperty mp = mo->property(i);
        const QString &propertyName = mp.name();
        mapProperty(this, propertyName);
    }
}

void QVariantMapper::refreshProperties()
{
    if(m_mappedProperties.isEmpty())
        return;

    m_propertiesRefresh = true;

    QMultiHashIterator<QObject*, MappedProperty> i(m_mappedProperties);
    while (i.hasNext())
    {
        i.next();
        QObject* object = i.key();
        MappedProperty mappedProperty = i.value();

        const QString key = mappedProperty.name;
        const QVariant previousValue = readProperty(object, mappedProperty.property);
        const QVariant currentValue = this->value(key, previousValue);

        if (this->contains(key) &&
            (!previousValue.isValid() || previousValue != currentValue))
        {
            writeProperty(object, mappedProperty.property, currentValue);
        }
    }

    m_propertiesRefresh = false;
}

void QVariantMapper::onPropertyChanged()
{
    if(m_selecting || m_propertiesRefresh)
        return;

    QObject *object = sender();
    if(!m_mappedProperties.contains(object))
        return;

    const int signalIndex = senderSignalIndex();
    if(signalIndex == -1)
        return;

    const QList<MappedProperty> mappedProperties = m_mappedProperties.values(object);
    for(const MappedProperty& mappedProperty: mappedProperties)
    {
        const QMetaProperty mp = mappedProperty.property;
        if(signalIndex==mp.notifySignalIndex())
        {
            const QVariant value = readProperty(object, mp);
            setValue(mappedProperty.name, value);
            QUTILSLOG_TRACE()<<"Property"<<object<<mappedProperty.name<<"updated in"<<this;
            return;
        }
    }
}

void QVariantMapper::onSelectInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_selectInhibitTimer.isActive();
    m_selectInhibitTimer.stop();
    m_selectInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_selectInhibitTimer.start();
}

void QVariantMapper::onSubmitInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_submitInhibitTimer.isActive();
    m_submitInhibitTimer.stop();
    m_submitInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_submitInhibitTimer.start();
}

void QVariantMapper::onRevertInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_submitInhibitTimer.isActive();
    m_revertInhibitTimer.stop();
    m_revertInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_revertInhibitTimer.start();
}

void QVariantMapper::onUpdateInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_updateInhibitTimer.isActive();
    m_updateInhibitTimer.stop();
    m_updateInhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_updateInhibitTimer.start();
}

const QStringList& QVariantMapper::getMappedKeys() const
{
    return m_mappedKeys;
}

void QVariantMapper::contentInvalidate()
{
    // if(m_selecting || m_submiting || m_reverting || m_updating)
    //     return;

    setIsDirty(m_storage!=m_cache || m_storage!=m_backup);

    emit this->storageChanged(m_storage);
    queueSubmit();
}

void QVariantMapper::alter(const QVariantMap &update)
{
    if(update.isEmpty())
        return;
    m_update.insert(update);
    queueUpdate();
}

const QVariantMap& QVariantMapper::getUpdate() const
{
    return m_update;
}

const QVariantMap& QVariantMapper::getSource() const
{
    return m_source;
}

void QVariantMapper::setSource(const QVariantMap& source)
{
    if(m_source==source)
        return;
    m_source = source;
    queueSelect();
    emit this->sourceChanged(m_source);
}

const QVariantMap& QVariantMapper::getStorage() const
{
    return m_storage;
}

const QVariantMap& QVariantMapper::getBackup() const
{
    return m_backup;
}

const QVariantMap& QVariantMapper::getCache() const
{
    return m_cache;
}

bool QVariantMapper::setStorage(const QVariantMap& storage)
{
    return replaceStorage(storage);
}

bool QVariantMapper::replaceStorage(const QVariantMap& storage)
{
    if(m_storage==storage)
        return false;

    QVariantMap map = storage;
    for(auto [key,value]: m_placeholder.asKeyValueRange())
    {
        if(!map.contains(key))
            map.insert(key, value);
    }
    m_storage = map;

    m_backup = m_storage;
    m_cache = m_storage;
    setIsDirty(false);

    emit this->storageChanged(m_storage);
    refreshProperties();

    return true;
}

bool QVariantMapper:: patchStorage(const QVariantMap& storage)
{
    if(storage.isEmpty())
        return false;

    for(auto [key,value]: storage.asKeyValueRange())
    {
        if(m_storage.contains(key) || m_placeholder.contains(key)) {
            m_storage.insert(key, value);
        }
    }

    m_backup = m_storage;
    m_cache = m_storage;
    setIsDirty(false);

    emit this->storageChanged(m_storage);
    refreshProperties();

    return true;
}

void QVariantMapper::queueSelect()
{
    if(!m_completed || !m_selectWhen)
        return;

    if (m_selectPolicy==QVariantMapperPolicies::Direct) {
        select();
    }
    else if (m_selectPolicy==QVariantMapperPolicies::Delayed) {
        if(!m_selectInhibitTimer.isActive()) {
            setSelectQueued(true);
            m_selectInhibitTimer.start();
        }
    }
    else if(m_selectPolicy==QVariantMapperPolicies::Postponed) {
        if(m_selectPostponed && !m_selectInhibitTimer.isActive()) {
            setSelectQueued(true);
            m_selectInhibitTimer.start();
        }
    }
    else if (m_selectPolicy==QVariantMapperPolicies::Manual) {
        setSelectQueued(true);
    }
}

bool QVariantMapper::select()
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

    if (m_selectPolicy==QVariantMapperPolicies::Disabled) {
        QUTILSLOG_WARNING()<<this<<"Select policy is disabled for"<<m_baseName;
        return false;
    }

    m_selectTimer.restart();

    if(!m_isReady)
        initProperties();
    m_isReady = true;

    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }

    setSelecting(true);
    setLoading(true);

    bool result = doSelect();

    return result;
}

bool QVariantMapper::waitForSelect(int timeout)
{
    if(!m_selecting && !m_selectQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantMapper::selectDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantMapper::doSelect()
{
    const QVariantMap storage = m_source;
    setStorage(storage);
    emitSelectDone(true);

    return true;
}

void QVariantMapper::queueSubmit()
{
    if(!m_completed || !m_submitWhen)
        return;

    if (m_submitPolicy==QVariantMapperPolicies::Direct) {
        submit();
    }
    else if (m_submitPolicy==QVariantMapperPolicies::Delayed) {
        if(!m_submitInhibitTimer.isActive()) {
            setSubmitQueued(true);
            m_submitInhibitTimer.start();
        }
    }
    else if(m_submitPolicy==QVariantMapperPolicies::Postponed) {
        if(m_submitPostponed && !m_submitInhibitTimer.isActive()) {
            setSubmitQueued(true);
            m_submitInhibitTimer.start();
        }
    }
    else if (m_submitPolicy==QVariantMapperPolicies::Manual) {
        setSubmitQueued(true);
    }
}

bool QVariantMapper::submit()
{
    setSubmitPostponed(true);
    setSubmitQueued(false);
    m_submitInhibitTimer.stop();

    if(!m_submitWhen)
        return false;

    if (m_submitPolicy==QVariantMapperPolicies::Disabled) {
        QUTILSLOG_WARNING()<<this<<"Submit policy is disabled for"<<m_baseName;
        return false;
    }

    if(!m_isDirty)
        return true;

    m_submitTimer.restart();

    setSubmiting(true);
    setLoading(true);

    QStringList dirtyKeys;
    for(auto [key, value]: m_storage.asKeyValueRange())
    {
        if(dirtyKeys.contains(key) || (m_backup.contains(key) && value==m_backup.value(key)) || (m_cache.contains(key) && value==m_cache.value(key)))
            continue;
        dirtyKeys.append(key);
    }
    bool result = doSubmit(dirtyKeys);

    m_cache = m_storage;

    return result;
}

bool QVariantMapper::waitForSubmit(int timeout)
{
    if(!m_submiting && !m_submitQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantMapper::submitDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantMapper::doSubmit(const QStringList& dirtyKeys)
{
    Q_UNUSED(dirtyKeys)

    m_source = m_storage;
    emit this->sourceChanged(m_source);

    emitSubmitDone(true);

    return true;
}

void QVariantMapper::queueRevert()
{
    if(!m_completed || !m_revertWhen)
        return;

    if (m_revertPolicy==QVariantMapperPolicies::Direct) {
        revert();
    }
    else if (m_revertPolicy==QVariantMapperPolicies::Delayed) {
        if(!m_revertInhibitTimer.isActive()) {
            setRevertQueued(true);
            m_revertInhibitTimer.start();
        }
    }
    else if(m_revertPolicy==QVariantMapperPolicies::Postponed) {
        if(m_revertPostponed && !m_revertInhibitTimer.isActive()) {
            setRevertQueued(true);
            m_revertInhibitTimer.start();
        }
    }
    else if (m_revertPolicy==QVariantMapperPolicies::Manual) {
        setRevertQueued(true);
    }
}

bool QVariantMapper::revert()
{
    setRevertPostponed(true);
    setRevertQueued(false);
    m_revertInhibitTimer.stop();

    if(!m_revertWhen)
        return false;

    if (m_revertPolicy==QVariantMapperPolicies::Disabled) {
        QUTILSLOG_WARNING()<<this<<"Revert policy is disabled for"<<m_baseName;
        return false;
    }

    m_revertTimer.restart();

    setReverting(true);
    setLoading(true);

    bool result = doRevert();

    return result;
}

bool QVariantMapper::waitForRevert(int timeout)
{
    if(!m_reverting && !m_revertQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantMapper::revertDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantMapper::doRevert()
{
    setStorage(m_backup);
    emitRevertDone(true);

    return true;
}

void QVariantMapper::queueUpdate()
{
    if(!m_completed || !m_updateWhen)
        return;

    if (m_updatePolicy==QVariantMapperPolicies::Direct) {
        update();
    }
    else if (m_updatePolicy==QVariantMapperPolicies::Delayed) {
        if(!m_updateInhibitTimer.isActive()) {
            setUpdateQueued(true);
            m_updateInhibitTimer.start();
        }
    }
    else if(m_updatePolicy==QVariantMapperPolicies::Postponed) {
        if(m_updatePostponed && !m_updateInhibitTimer.isActive()) {
            setUpdateQueued(true);
            m_updateInhibitTimer.start();
        }
    }
    else if (m_updatePolicy==QVariantMapperPolicies::Manual) {
        setUpdateQueued(true);
    }
}

bool QVariantMapper::update()
{
    setUpdatePostponed(true);
    setUpdateQueued(false);
    m_updateInhibitTimer.stop();

    if(!m_updateWhen)
        return false;

    if (m_updatePolicy==QVariantMapperPolicies::Disabled) {
        QUTILSLOG_WARNING()<<this<<"Update policy is disabled for"<<m_baseName;
        return false;
    }

    m_updateTimer.restart();

    setUpdating(true);
    setLoading(true);

    bool result = doUpdate();

    return result;
}

bool QVariantMapper::waitForUpdate(int timeout)
{
    if(!m_updating && !m_updateQueued)
        return true;

    QEventLoop loop;
    connect(this, &QVariantMapper::updateDone, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(QEventLoop::AllEvents);
}

bool QVariantMapper::doUpdate()
{
    if(m_update.isEmpty()) {
        emitUpdateDone(true);
        return true;
    }

    patchStorage(m_update);
    m_update.clear();

    emitUpdateDone(true);

    return true;
}

void QVariantMapper::emitSelectDone(bool result)
{
    setSelected(true);

    setIsDirty(false);
    setSelecting(false);
    setLoading(false);

    QUTILSLOG_DEBUG()<<this<<m_baseName<<"select took:"<<m_selectTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->selectDone(result);
}

void QVariantMapper::emitSubmitDone(bool result)
{
    m_backup = m_storage;
    m_cache = m_storage;
    setIsDirty(false);
    setSubmiting(false);
    setLoading(false);

    QUTILSLOG_DEBUG()<<this<<m_baseName<<"submit took:"<<m_submitTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->submitDone(result);
}

void QVariantMapper::emitRevertDone(bool result)
{
    setIsDirty(false);
    setReverting(false);
    setLoading(false);

    QUTILSLOG_DEBUG()<<this<<m_baseName<<"revert took:"<<m_revertTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->revertDone(result);
}

void QVariantMapper::emitUpdateDone(bool result)
{
    setIsDirty(false);
    setUpdating(false);
    setLoading(false);

    QUTILSLOG_DEBUG()<<this<<m_baseName<<"update took:"<<m_updateTimer.nsecsElapsed()/1000000.0<<"with result:"<<result;

    emit this->updateDone(result);
}

QVariant QVariantMapper::readProperty(const QObject* object, const QMetaProperty &property)
{
    QVariant value = property.read(object);
    if (property.isEnumType())
        value = value.toInt();
    else if (value.userType() == qMetaTypeId<QJSValue>())
        value = value.value<QJSValue>().toVariant();
    return value;
}

bool QVariantMapper::writeProperty(QObject *object, const QMetaProperty &property, const QVariant &value)
{
    if(property.write(object, value))
        return true;

    QUTILSLOG_DEBUG()<<"Cannot write"<<value<<"to property"<<property.name()<<"of type"<<property.metaType().name();

    return property.write(object, QVariant(property.metaType()));
}
