#ifndef QMODELHELPER_H
#define QMODELHELPER_H

#include <QIdentityProxyModel>
#include <QStandardItemModel>
#include <QQmlPropertyMap>
#include <QQmlEngine>

class QModelHelper : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModelHelper)
    QML_UNCREATABLE("Attached")
    QML_ATTACHED(QModelHelper)

    // ──────── CONSTRUCTOR ──────────
private:
    explicit QModelHelper(QAbstractItemModel* object);

public:
    static QModelHelper* wrap(const QObject* object);
    static QModelHelper* qmlAttachedProperties(QObject* object);

    // ──────── ABSTRACT MODEL OVERRIDE ──────────
public:
    Q_INVOKABLE int roleForName(const QByteArray& name) const;
    Q_INVOKABLE QByteArray roleName(int role) const;

    // ──────── COUNT PUBLIC API ──────────
private:
    Q_PROPERTY(int content READ count NOTIFY contentChanged FINAL)
    Q_PROPERTY(int contentIsEmpty READ isEmpty NOTIFY contentChanged FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

protected slots:
    void countInvalidate();

signals:
    void contentChanged();
    void countChanged(int count);
    void emptyChanged(bool empty);

    // ──────── MAP PUBLIC API ──────────
public:
    Q_INVOKABLE QQmlPropertyMap* map(int row, int column = 0, const QModelIndex& parent = {});

    // ──────── PUBLIC API ──────────
public:
    int count() const { return m_model->rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    Q_INVOKABLE bool set(int row, const QVariantMap& map);
    Q_INVOKABLE bool setProperty(int row, const QString& property, const QVariant& value);
    Q_INVOKABLE bool setProperties(const QString& property, const QVariant& value);
    Q_INVOKABLE bool setProperties(const QList<int>& indexes, const QString& property, const QVariant& value);
    Q_INVOKABLE QVariantMap find(const QString &columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder) const;
    Q_INVOKABLE QVariantMap get(int row, const QStringList& roles = QStringList()) const;
    Q_INVOKABLE QVariant getProperty(int row, const QString& property) const;
    Q_INVOKABLE QVariantList getProperties(const QString& property) const;
    Q_INVOKABLE QVariantList getProperties(const QList<int>& indexes, const QString& property) const;
    Q_INVOKABLE QVariantList getProperties(const QModelIndexList& indexes, const QString& property) const;

    Q_INVOKABLE bool isSorted(int role, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE bool isSorted(const QString& roleName, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int isSortedUntil(int role, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int isSortedUntil(const QString& roleName, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;

    Q_INVOKABLE int lowerBound(const QString &roleName, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int lowerBound(int role, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int upperBound(const QString &roleName, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int upperBound(int role, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;

    Q_INVOKABLE int indexOf(const QString &columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE QList<int> indexesOf(const QString &columnName, const QVariant& val, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false) const;
    Q_INVOKABLE int count(const QString &columnName, const QVariant& val) const;
    Q_INVOKABLE bool contains(const QString& columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder) const;
    Q_INVOKABLE bool containsExclude(const QString& columnName, const QVariant& val, const QString& excludedColumnName, const QVariant& excludedVal) const;

    Q_INVOKABLE bool equals(const QAbstractItemModel* model) const;
    Q_INVOKABLE QVariantList toVariantList() const;
    Q_INVOKABLE QVariantList toFilteredVariantList(const QString &columnName, const QVariantList& values) const;

    Q_INVOKABLE bool dataDiffersFromRowSibling(int row, int column, int role = Qt::DisplayRole) const;
    Q_INVOKABLE bool dataDiffersFromColumnSibling(int row, int column, int role = Qt::DisplayRole) const;

    // ──────── PUBLIC STATIC API ──────────
public:
    static int roleForName(const QAbstractItemModel* model, const QByteArray& name);
    static QByteArray roleName(const QAbstractItemModel* model, int role);

    static int count(const QAbstractItemModel* model) { return model->rowCount(); };
    static int size(const QAbstractItemModel* model) { return count(model); };
    static int length(const QAbstractItemModel* model) { return count(model); };
    static bool isEmpty(const QAbstractItemModel* model) { return count(model) == 0; };

    static bool set(QAbstractItemModel* model, int row, const QVariantMap& map);
    static bool setProperty(QAbstractItemModel* model, int row, const QString& property, const QVariant& value);
    static bool setProperties(QAbstractItemModel* model, const QString& property, const QVariant& value);
    static bool setProperties(QAbstractItemModel* model, const QList<int>& indexes, const QString& property, const QVariant& value);
    static QVariantMap find(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder);
    static QVariantMap get(const QAbstractItemModel* model, int row, const QStringList& roles = QStringList());
    static QVariant getProperty(const QAbstractItemModel* model, int row, const QString& property);
    static QVariantList getProperties(const QAbstractItemModel* model, const QString& property);
    static QVariantList getProperties(const QAbstractItemModel* model, const QList<int>& indexes, const QString& property);
    static QVariantList getProperties(const QAbstractItemModel* model, const QModelIndexList& indexes, const QString& property);

    static bool isSorted(const QAbstractItemModel* model, int role, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static bool isSorted(const QAbstractItemModel* model, const QString& roleName, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int isSortedUntil(const QAbstractItemModel* model, int role, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int isSortedUntil(const QAbstractItemModel* model, const QString& roleName, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);

    static int lowerBound(const QAbstractItemModel* model, const QString& roleName, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int lowerBound(const QAbstractItemModel* model, int role, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int upperBound(const QAbstractItemModel* model, const QString& roleName, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int upperBound(const QAbstractItemModel* model, int role, const QVariant& value, Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);

    static int indexOf(const QAbstractItemModel* model, int role, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static QList<int> indexesOf(const QAbstractItemModel* model, int role, const QVariant& val, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int count(const QAbstractItemModel* model, int role, const QVariant& val);
    static bool contains(const QAbstractItemModel* model, int role, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder);
    static bool containsExclude(const QAbstractItemModel* model, int role, const QVariant& val, const QString& excludedColumnName, const QVariant& excludedVal);

    static int indexOf(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder, bool upperBound=false, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static QList<int> indexesOf(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false);
    static int count(const QAbstractItemModel* model, const QString &columnName, const QVariant& val);
    static bool contains(const QAbstractItemModel* model, const QString& columnName, const QVariant& val, bool isSorted=false, Qt::SortOrder sortOrder=Qt::AscendingOrder);
    static bool containsExclude(const QAbstractItemModel* model, const QString& columnName, const QVariant& val, const QString& excludedColumnName, const QVariant& excludedVal);

    static bool equals(const QAbstractItemModel* srcModel, const QAbstractItemModel* compModel);
    static QVariantList toVariantList(const QAbstractItemModel* model);
    static QVariantList toFilteredVariantList(const QAbstractItemModel* model, const QString &columnName, const QVariantList& values);

    static bool dataDiffersFromRowSibling(const QAbstractItemModel* model, int row, int column, int role = Qt::DisplayRole);
    static bool dataDiffersFromColumnSibling(const QAbstractItemModel* model, int row, int column, int role = Qt::DisplayRole);

    // ──────── HELPER PRIVATE ──────────
private:
    QQmlPropertyMap* mapperForRow(int row) const;
    void removeMapper(QObject* mapper);

    // ──────── ATTRIBUTES ──────────
private:
    QAbstractItemModel* m_model;

    int m_count=0;
    QVector<QPair<int, QQmlPropertyMap*>> m_mappers;
};

//────────────────────────────────────────────────────────────────────────────────────────────────
// QModelHelperPropertyMap
//────────────────────────────────────────────────────────────────────────────────────────────────

class QModelHelperPropertyMap: public QQmlPropertyMap
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModelHelperPropertyMap)
    QML_UNCREATABLE("Abstract")

public:
    explicit QModelHelperPropertyMap(int row, int column, const QModelIndex& parentIndex, QAbstractItemModel* model, QObject* parent = nullptr);

protected:
    QVariant updateValue(const QString& key, const QVariant& input) override;

private:
    QModelIndex modelIndex() const;

    void update();

    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onRowsRemoved(const QModelIndex& parent, int first, int last);
    void onColumnsInserted(const QModelIndex& parent, int first, int last);
    void onColumnsRemoved(const QModelIndex& parent, int first, int last);

    int m_row;
    int m_column;
    QPersistentModelIndex m_parent;
    QAbstractItemModel* m_model;
};

#endif // QMODELHELPER_H
