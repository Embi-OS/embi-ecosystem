#ifndef QVARIANTREADERMODEL_H
#define QVARIANTREADERMODEL_H

#include <QDefs>

#include "qvariantreader.h"
#include <QAbstractListModel>
#include <QQmlParserStatus>
class QVariantReaderModel: public QAbstractListModel,
                           public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VariantReaderModel)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, forwardOnly, ForwardOnly, false)
    Q_WRITABLE_VAR_PROPERTY(bool, estimateCount, EstimateCount, false)
    Q_WRITABLE_REF_PROPERTY(QStringList, roles, Roles, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, placeholder, Placeholder, {})

    Q_READONLY_VAR_PROPERTY(int, code, Code, 0)
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})
    Q_READONLY_REF_PROPERTY(QString, message, Message, {})

public:
    explicit QVariantReaderModel(QObject* parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    QVariantReader* reader() const;

    int count() const { return rowCount(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE int roleForName(const QString& name) const;
    Q_INVOKABLE QByteArray roleName(int role) const;
    void clearRoleNames();
    void updateRoleNames(const QVariant& var);

public slots:
    bool setCbor(const QByteArray& cbor);
    bool setJson(const QByteArray& json);
    bool clear();
    bool seek(int index);

signals:
    void completedChanged(bool completed);

    void countChanged(int count);
    void emptyChanged(bool empty);

    void error();

private:
    void countInvalidate();
    bool initilized();

    bool m_completed=false;
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;
    QHash<QString, int> m_roleIds;

    QVariantList m_cache;

    QVariantReader* m_reader=nullptr;
    int m_bottom=0;
    int m_rowCount=0;
    bool m_initialized=false;
};

#endif // QVARIANTREADERMODEL_H
