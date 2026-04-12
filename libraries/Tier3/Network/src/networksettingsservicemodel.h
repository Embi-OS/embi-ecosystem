#ifndef NETWORKSETTINGSSERVICEMODEL_H
#define NETWORKSETTINGSSERVICEMODEL_H

#include <QDefs>
#include <QSortFilterProxyModel>
#include "networksettings.h"

class NetworkSettingsService;
class NetworkSettingsServiceModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsServiceModel(QObject *parent = nullptr);
    virtual ~NetworkSettingsServiceModel();
    // from QAbstractItemModel
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool replacePlaceholder(NetworkSettingsService* networkService);
    void append(NetworkSettingsService* networkService);
    void insert(int row, NetworkSettingsService* networkInterface);
    void remove(int row);
    bool removeService(const QString &id);
    void updated(int row);
    NetworkSettingsService* getByName(const QString& name);
    QList<NetworkSettingsService*> getModel();

    enum Roles {
        Type = Qt::UserRole + 1,
        State,
        Name,
        Identifier,
        SignalStrength,
        Connected,
        Interface,
        Address,
        MAC
    };


private Q_SLOTS:
    void connectionStatusChanged();
    void signalStrengthChanged();

private:
    void connectStateChanges(NetworkSettingsService* item);

    QList<NetworkSettingsService*> m_items;
    QHash<int, QByteArray> m_roleNames;
};

class NetworkSettingsServiceFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(NetworkSettingsType::Type type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(bool wiredNetworksAvailable MEMBER m_wiredNetworksAvailable WRITE setWiredNetworksAvailable NOTIFY wiredNetworksAvailableChanged FINAL)
public:
    explicit NetworkSettingsServiceFilter(QObject* parent = nullptr);
    virtual ~NetworkSettingsServiceFilter();
    bool filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const override;
    NetworkSettingsType::Type type() const;
    void setType(NetworkSettingsType::Type type);
    Q_INVOKABLE QVariant itemFromRow(const int row) const;
    Q_INVOKABLE int activeRow() const;
    void setWiredNetworksAvailable(bool);
Q_SIGNALS:
    void typeChanged();
    void wiredNetworksAvailableChanged();
private:
    NetworkSettingsType::Type m_type;
    bool m_wiredNetworksAvailable;
};

#endif // NETWORKSETTINGSSERVICEMODEL_H
