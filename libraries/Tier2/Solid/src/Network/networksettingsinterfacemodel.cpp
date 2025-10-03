#include "networksettingsinterfacemodel.h"
#include "networksettingsinterface.h"

/*!
    \class NetworkSettingsInterfaceModel
    \inmodule QtNetworkSettings
    \brief The NetworkSettingsInterfaceModel class represents a network
    interface model.

    The network interface model contains a list of network interfaces
    attached to the host.
*/

/*!
    \enum NetworkSettingsInterfaceModel::Roles
    \brief This enum type holds information about a network interface.

    \value  Type
            Network interface \l{NetworkSettingsType::Type}{type}.
    \value  Status
            Network interface \l{NetworkSettingsState::State}{state}.
    \value  Name
            Network interface name.
    \value  Powered
            Whether the network interface is powered on or off.
*/

/*!
    Creates a new network interface model with the parent \a parent.
*/
NetworkSettingsInterfaceModel::NetworkSettingsInterfaceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames.insert(Qt::UserRole, "entry");
    m_roleNames.insert(Type, "type");
    m_roleNames.insert(State, "state");
    m_roleNames.insert(Name, "name");
    m_roleNames.insert(Powered, "powered");
}

/*!
    Returns an array of the names of the roles in the model.
*/
QHash<int, QByteArray> NetworkSettingsInterfaceModel::roleNames() const
{
    return m_roleNames;
}

/*!
    Returns the number of rows in the model with the parent \a parent.
*/
int NetworkSettingsInterfaceModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

/*!
    Returns the data at the index \a index in the model for the type of data
    specified by \a role.
*/
QVariant NetworkSettingsInterfaceModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) return QVariant();

    NetworkSettingsInterface *item = m_items[index.row()];
    if (role == Qt::UserRole) {
        return QVariant::fromValue(static_cast<QObject*>(item));
    }

    switch (role) {
    case Type:
        return item->type();
        break;
    case Name:
        return item->name();
        break;
    case State:
        return item->state();
        break;
    case Powered:
        return item->powered();
        break;
    default:
        return QStringLiteral("");
    }

}

/*!
    Appends \a item to the model.
*/
void NetworkSettingsInterfaceModel::append(NetworkSettingsInterface* item)
{
    insert(rowCount(), item);
}

/*!
    Inserts \a item into \a row in the model.
*/
void NetworkSettingsInterfaceModel::insert(int row, NetworkSettingsInterface* item)
{
    item->setParent(this);
    connectStateChanges(item);

    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

void NetworkSettingsInterfaceModel::connectStateChanges(NetworkSettingsInterface* item)
{
    connect(item, &NetworkSettingsInterface::stateChanged, this, &NetworkSettingsInterfaceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsInterface::poweredChanged, this, &NetworkSettingsInterfaceModel::poweredChanged);
}

/*!
    Removes the row \a row from the model.
*/
void NetworkSettingsInterfaceModel::remove(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_items.takeAt(row)->deleteLater();
    endRemoveRows();
}

/*!
    Returns whether the interface with the \a name was removed successfully.
*/
bool NetworkSettingsInterfaceModel::removeInterface(const QString &name)
{
    bool ret = false;
    for (int i=0; i < m_items.count(); i++) {
       if (m_items.at(i)->name() == name) {
           remove(i);
           ret = true;
           break;
       }
    }
    return ret;
}

/*!
    Notifies that the \a row has been updated.
*/
void NetworkSettingsInterfaceModel::updated(int row)
{
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}

/*!
    Returns the network interface model.
*/
QList<NetworkSettingsInterface*> NetworkSettingsInterfaceModel::getModel()
{
    return m_items;
}

void NetworkSettingsInterfaceModel::connectionStatusChanged()
{
    NetworkSettingsInterface *s = qobject_cast<NetworkSettingsInterface*>(sender());

    int row = 0;
    for (NetworkSettingsInterface* item: m_items) {
        if (item == s) {
            updated(row);
            break;
        }
        row++;
    }

}

void NetworkSettingsInterfaceModel::poweredChanged()
{
    NetworkSettingsInterface *s = qobject_cast<NetworkSettingsInterface*>(sender());
    int row = 0;
    for (NetworkSettingsInterface* item: m_items) {
        if (item == s) {
            updated(row);
            break;
        }
        row++;
    }
}
