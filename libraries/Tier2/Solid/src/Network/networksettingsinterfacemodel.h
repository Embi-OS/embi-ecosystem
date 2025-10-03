#ifndef NETWORKSETTINGSINTERFACEMODEL_H
#define NETWORKSETTINGSINTERFACEMODEL_H

#include <QDefs>
#include <QAbstractListModel>

class NetworkSettingsInterface;
class NetworkSettingsInterfaceModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsInterfaceModel(QObject *parent = nullptr);
    // from QAbstractItemModel
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void append(NetworkSettingsInterface* networkInterface);
    void insert(int row, NetworkSettingsInterface* networkInterface);
    void remove(int row);
    QList<NetworkSettingsInterface*> getModel();

    enum Roles {
        Type = Qt::UserRole + 1,
        State,
        Name,
        Powered
    };

    void updated(int row);
    bool removeInterface(const QString &name);
private Q_SLOTS:
    void connectionStatusChanged();
    void poweredChanged();

private:
    void connectStateChanges(NetworkSettingsInterface* item);

private:
    QList<NetworkSettingsInterface*> m_items;
    QHash<int, QByteArray> m_roleNames;
};

#endif // NETWORKSETTINGSINTERFACEMODEL_H
