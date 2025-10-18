#ifndef QVARIANTMAPPER_H
#define QVARIANTMAPPER_H

#include <QObject>
#include <QVariant>
#include <QElapsedTimer>
#include <QQmlParserStatus>
#include <QDefs>

Q_ENUM_CLASS(QVariantMapperPolicies, QVariantMapperPolicy,
             Direct,
             Delayed,
             Postponed,
             Manual,
             Disabled)

class QVariantMapper: public QObject,
                      public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VariantMapper)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_PROPERTY(QVariantMap storage READ getStorage NOTIFY storageChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QString, baseName, BaseName, {})
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, roles, Roles, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, placeholder, Placeholder, {})
    Q_PROPERTY(QVariantMap source READ getSource WRITE setSource NOTIFY sourceChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, syncable, Syncable, false)
    Q_WRITABLE_VAR_PROPERTY(QVariantMapperPolicies::Enum, selectPolicy, SelectPolicy, QVariantMapperPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantMapperPolicies::Enum, submitPolicy, SubmitPolicy, QVariantMapperPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantMapperPolicies::Enum, revertPolicy, RevertPolicy, QVariantMapperPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(QVariantMapperPolicies::Enum, updatePolicy, UpdatePolicy, QVariantMapperPolicies::Direct)

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

public:
    explicit QVariantMapper(QObject * parent = nullptr);
    explicit QVariantMapper(const QString& baseName, QObject * parent = nullptr, const QMetaObject* blacklistedMetaObject = &QVariantMapper::staticMetaObject);
    ~QVariantMapper() override;

    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE bool setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE bool reset();
    Q_INVOKABLE bool clear();
    Q_INVOKABLE QVariantMap values(QStringList keys = QStringList()) const;
    Q_INVOKABLE void remove(const QString &key);
    Q_INVOKABLE bool contains(const QString &key);
    Q_INVOKABLE QStringList keys() const;
    Q_INVOKABLE QStringList keys(const QVariant &value) const;
    Q_INVOKABLE void alter(const QVariantMap &update);

    Q_INVOKABLE void mapObject(QObject* object);
    Q_INVOKABLE bool mapProperty(QObject* object, const QString& property, const QString& aliasName="");

    const QVariantMap& getStorage() const;
    const QVariantMap& getBackup() const;
    const QVariantMap& getCache() const;
    const QVariantMap& getSource() const;
    const QVariantMap& getUpdate() const;
    const QStringList& getMappedKeys() const;

public slots:
    void setSource(const QVariantMap& source);

    void queueSelect();
    bool select();
    virtual bool waitForSelect(int timeout=-1);

    void queueSubmit();
    bool submit();
    virtual bool waitForSubmit(int timeout=-1);

    void queueRevert();
    bool revert();
    virtual bool waitForRevert(int timeout=-1);

    void queueUpdate();
    bool update();
    virtual bool waitForUpdate(int timeout=-1);

signals:
    void completedChanged(bool completed);

    void valueChanged(const QString &key, const QVariant &value);
    void storageChanged(const QVariantMap& storage);
    void sourceChanged(const QVariantMap& source);

    void selectDone(bool result);
    void submitDone(bool result);
    void revertDone(bool result);
    void updateDone(bool result);

protected slots:
    void clearProperties();
    void initProperties();
    void refreshProperties();

    void onPropertyChanged();

    void emitSelectDone(bool result);
    void emitSubmitDone(bool result);
    void emitRevertDone(bool result);
    void emitUpdateDone(bool result);

    void onSelectInhibitTimeChanged(int inhibitTime);
    void onSubmitInhibitTimeChanged(int inhibitTime);
    void onRevertInhibitTimeChanged(int inhibitTime);
    void onUpdateInhibitTimeChanged(int inhibitTime);

protected:
    void contentInvalidate();
    bool setStorage(const QVariantMap& storage);
    bool replaceStorage(const QVariantMap& storage);
    bool patchStorage(const QVariantMap& storage);

    virtual bool doSelect();
    virtual bool doSubmit(const QStringList& dirtyKeys);
    virtual bool doRevert();
    virtual bool doUpdate();

    const QMetaObject* m_blacklistedMetaObject;

private:
    static QVariant readProperty(const QObject* object, const QMetaProperty &property);
    static bool writeProperty(QObject *object, const QMetaProperty &property, const QVariant &value);

    struct MappedProperty {
        MappedProperty(const QString& n, const QMetaProperty& mp) :
            name(n),
            property(mp)
        {}

        QString name;
        QMetaProperty property;

        bool operator==(const MappedProperty& rhs) const { return name==rhs.name; }
        bool operator!=(const MappedProperty& rhs) const { return !(*this==rhs); }
    };

    QMultiHash<QObject*, MappedProperty> m_mappedProperties;
    bool m_propertiesRefresh = false;

    bool m_completed=false;
    bool m_isReady=false;

    QVariantMap m_storage;
    QVariantMap m_backup;
    QVariantMap m_cache;
    QVariantMap m_source;
    QVariantMap m_update;

    QStringList m_mappedKeys;

    QTimer m_selectInhibitTimer;
    QTimer m_submitInhibitTimer;
    QTimer m_revertInhibitTimer;
    QTimer m_updateInhibitTimer;

    QElapsedTimer m_selectTimer;
    QElapsedTimer m_submitTimer;
    QElapsedTimer m_revertTimer;
    QElapsedTimer m_updateTimer;
};

#endif // QVARIANTMAPPER_H
