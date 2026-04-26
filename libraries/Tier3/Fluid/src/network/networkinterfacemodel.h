#ifndef NETWORKINTERFACEMODEL_H
#define NETWORKINTERFACEMODEL_H

#include <QNetworkInterface>
#include <QAbstractListModel>
#include <QDefs>

class NetworkInterfaceModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(int, refreshInterval, RefreshInterval, 0)

public:
    explicit NetworkInterfaceModel(QObject *parent = nullptr);

    enum {
        Type = Qt::UserRole + 1,
        Name,
        Identifier,
        Connected,
        Up,
        Running,
        Address,
        MAC
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    Q_INVOKABLE bool contains(const QString& name) const;

public slots:
    void queueRefresh();
    void refresh();

signals:
    void countChanged(int count);
    void emptyChanged(bool empty);

private slots:
    void countInvalidate();

private:
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;
    bool m_refreshQueued = false;

    QList<QNetworkInterface> m_interfaces;
};

#endif // NETWORKINTERFACEMODEL_H
