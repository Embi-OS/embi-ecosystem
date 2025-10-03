#include "networksettingsservicemodel.h"
#include "networksettingsservice.h"
#include "networksettings.h"

/*!
    \class NetworkSettingsServiceModel
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsServiceModel class represents a network service.

    The network service model contains a list of network services provided by
    the host.

    \sa NetworkSettingsService
*/

/*!
    \enum NetworkSettingsServiceModel::Roles

    This enum type holds information about the network connection.

    \value  Type
            Network \l{NetworkSettingsType::Type}{type}.
    \value  Name
            The service set identifier (SSID) of the network.
    \value  SignalStrength
            The signal strength of the connection.
    \value  Connected
            Whether the connection has been established.
*/

/*!
    Creates a network service model with the parent \a parent.
*/
NetworkSettingsServiceModel::NetworkSettingsServiceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames.insert(Qt::UserRole, "entry");
    m_roleNames.insert(Type, "type");
    m_roleNames.insert(State, "state");
    m_roleNames.insert(Name, "name");
    m_roleNames.insert(Identifier, "identifier");
    m_roleNames.insert(SignalStrength, "signalStrength");
    m_roleNames.insert(Connected, "connected");
    m_roleNames.insert(Interface, "iface");
    m_roleNames.insert(Address, "address");
    m_roleNames.insert(MAC, "mac");
}

/*!
    Deletes the network service model.
*/
NetworkSettingsServiceModel::~NetworkSettingsServiceModel()
{

}

/*!
    Returns an array of the names of the roles in the model.
*/
QHash<int, QByteArray> NetworkSettingsServiceModel::roleNames() const
{
    return m_roleNames;
}

/*!
    Returns the number of rows in the model with the parent \a parent.
*/
int NetworkSettingsServiceModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

/*!
    Returns the data at the index \a index in the model for the type of data
    specified by \a role.
*/
QVariant NetworkSettingsServiceModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) return QVariant();

    NetworkSettingsService *item = m_items[index.row()];
    if (role == Qt::UserRole) {
        return QVariant::fromValue(item);
    }
    else if (role == Type) {
        return item->type();
    }
    else if (role == State) {
        return item->state();
    }
    else if (role == Name) {
        return item->name();
    }
    else if (role == Identifier) {
        return item->id();
    }
    else if (role == MAC) {
        return item->ethernetConfig()->getAddress();
    }
    else if (role == SignalStrength) {
        return item->wirelessConfig()->signalStrength();
    }
    else if (role == Connected) {
        return item->state() == NetworkSettingsState::Online || item->state() == NetworkSettingsState::Ready;
    }
    else if (role == Interface) {
        return item->ethernetConfig()->getInterface();
    }
    else if (role == Address) {
        if(item->ipv4())
            return item->ipv4()->getAddress();
        return "N/A";
    }
    return QVariant();
}

/*!
    Replaces placeholder data with \a item. Returns \c true on success.
*/
bool NetworkSettingsServiceModel::replacePlaceholder(NetworkSettingsService* item)
{
    if (item->type() == NetworkSettingsType::Wired) {
        for (int i = 0; i < m_items.size(); ++i) {
            NetworkSettingsService* existing = m_items.at(i);
            if (existing->placeholderState()) {
                m_items.replace(i, item);
                existing->deleteLater();
                updated(i);
                return true;
            }
        }
    }
    return false;
}

/*!
    Appends \a item to the model.
*/
void NetworkSettingsServiceModel::append(NetworkSettingsService* item)
{
    item->setParent(this);
    connectStateChanges(item);

    if ((item->type() == NetworkSettingsType::Wired) && replacePlaceholder(item)) {
        return;
    }

    beginResetModel();
    m_items.append(item);
    endResetModel();
}

/*!
    Inserts \a item into \a row in the model.
*/
void NetworkSettingsServiceModel::insert(int row, NetworkSettingsService* item)
{
    item->setParent(this);
    connectStateChanges(item);

    if ((item->type() == NetworkSettingsType::Wired) && replacePlaceholder(item)) {
        return;
    }

    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

void NetworkSettingsServiceModel::connectStateChanges(NetworkSettingsService* item)
{
    connect(item, &NetworkSettingsService::typeChanged, this, &NetworkSettingsServiceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsService::stateChanged, this, &NetworkSettingsServiceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsService::nameChanged, this, &NetworkSettingsServiceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsService::ipv4Changed, this, &NetworkSettingsServiceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsService::ipv6Changed, this, &NetworkSettingsServiceModel::connectionStatusChanged);
    connect(item, &NetworkSettingsService::ethernetChanged, this, &NetworkSettingsServiceModel::connectionStatusChanged);

    NetworkSettingsWireless* wireless = item->wirelessConfig();
    if (wireless)
        connect(wireless, &NetworkSettingsWireless::signalStrengthChanged, this, &NetworkSettingsServiceModel::signalStrengthChanged);
}

/*!
    Removes the row \a row from the model.
*/
void NetworkSettingsServiceModel::remove(int row)
{
    NetworkSettingsService* item = m_items.at(row);
    if (item->type() == NetworkSettingsType::Wired) {
        /* Don't remove a wired service so that it doesn't become undefined in the UI.
           This avoids problems when a cable is disconnected. */
        item->setPlaceholderState(true);
        return;
    }
    item->deleteLater();
    beginRemoveRows(QModelIndex(), row, row);
    m_items.removeAt(row);
    endRemoveRows();
}

/*!
    Removes the service specified by \a id from the model. Returns \c true if the service
    was successfully removed, \c false otherwise.
*/
bool NetworkSettingsServiceModel::removeService(const QString &id)
{
    bool ret = false;
    for (int i=0; i < m_items.count(); i++) {
       if (m_items.at(i)->id() == id) {
           remove(i);
           ret = true;
           break;
       }
    }
    return ret;
}

/*!
    Marks the data on the row \a row in the model as updated.
*/
void NetworkSettingsServiceModel::updated(int row)
{
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
}

/*!
    Returns the service with the name \a name.
*/
NetworkSettingsService* NetworkSettingsServiceModel::getByName(const QString& name)
{
    NetworkSettingsService* ret = nullptr;
    for (NetworkSettingsService* item: std::as_const(m_items)) {
        if (item->name() == name) {
            ret = item;
            break;
        }
    }
    return ret;
}

/*!
    Returns the network service model.
*/
QList<NetworkSettingsService*> NetworkSettingsServiceModel::getModel()
{
    return m_items;
}

/*!
    This signal is emitted when the connection status changes.
*/
void NetworkSettingsServiceModel::connectionStatusChanged()
{
    NetworkSettingsService *s = qobject_cast<NetworkSettingsService*>(sender());

    int row = 0;
    for (NetworkSettingsService* item: std::as_const(m_items)) {
        if (item == s) {
            updated(row);
            break;
        }
        row++;
    }

}

/*!
    This signal is emitted when the signal strength changes.
*/
void NetworkSettingsServiceModel::signalStrengthChanged()
{
    NetworkSettingsWireless *s = qobject_cast<NetworkSettingsWireless*>(sender());
    int row = 0;
    for (NetworkSettingsService* item: std::as_const(m_items)) {
        if (item->wirelessConfig() == s) {
            updated(row);
            break;
        }
        row++;
    }
}

//Filter model

/*!
    \class NetworkSettingsServiceFilter
    \inmodule QtNetworkSettings

    \brief The NetworkSettingsServiceFilter class represents a network service
    filter.

    \sa NetworkSettingsService
*/

/*!
    \property NetworkSettingsServiceFilter::type
    \brief The type of the network.

    \l NetworkSettingsType::Type
*/

/*!
    \property NetworkSettingsServiceFilter::wiredNetworksAvailable
    \brief Whether wired networks are available.
*/

/*!
    \fn NetworkSettingsServiceFilter::typeChanged()

    This signal is emitted when the network type changes.
*/

/*!
    \fn NetworkSettingsServiceFilter::wiredNetworksAvailableChanged()

    This signal is emitted when the availability of wired networks changes.
*/

/*!
    \qmltype NetworkSettingsServiceFilter
    \inqmlmodule QtDeviceUtilities.NetworkSettings
    \abstract
*/

/*!
    Creates a network service filter with the parent \a parent.
*/
NetworkSettingsServiceFilter::NetworkSettingsServiceFilter(QObject* parent)
    :QSortFilterProxyModel(parent), m_type(NetworkSettingsType::Unknown)
{
    connect(this, &NetworkSettingsServiceFilter::typeChanged, this, &NetworkSettingsServiceFilter::invalidate);
}

/*!
    Deletes the network service filter.
*/
NetworkSettingsServiceFilter::~NetworkSettingsServiceFilter()
{

}

/*!
    \qmlproperty enumeration NetworkSettingsServiceFilter::type

    \value NetworkSettingsType.Wired Wired network
    \value NetworkSettingsType.Wifi Wifi network
    \value NetworkSettingsType.Bluetooth Bluetooth network
    \value NetworkSettingsType.Unknown Unknown network type
*/

/*!
    Returns the service model.

    \l NetworkSettingsType::Type
*/
NetworkSettingsType::Type  NetworkSettingsServiceFilter::type() const
{
    return m_type;
}

/*!
    \fn void NetworkSettingsServiceFilter::setType(NetworkSettingsType::Type type)

    Sets the service model to \a type.
*/
void NetworkSettingsServiceFilter::setType(const NetworkSettingsType::Type type)
{
    m_type = type;
    emit typeChanged();
}

/*!
    Returns whether the row \a source_row has the user role and whether it is
    found in the model \a source_parent.
*/
bool NetworkSettingsServiceFilter::filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const
{
    if (this->sourceModel())
    {
       QModelIndex index = this->sourceModel()->index( source_row, 0, source_parent );
       if (index.isValid())
       {
           QObject * obj = qvariant_cast<QObject *>(index.data(Qt::UserRole));
           NetworkSettingsService * service = qobject_cast<NetworkSettingsService *>(obj);
           if (service->type() == m_type || m_type == NetworkSettingsType::Unknown)
              return true;
       }
    }
    return false;
}

/*!
    \qmlmethod NetworkService NetworkSettingsServiceFilter::itemFromRow(int index)

    Returns the service at \a index in the model.
*/

/*!
    Returns the service at \a row in the model.
*/
QVariant NetworkSettingsServiceFilter::itemFromRow(const int row) const
{
    QModelIndex idx = index(row, 0);
    QModelIndex mapped = mapToSource(idx);
    if (mapped.isValid())
    {
        QVariant serviceItem = mapped.data(Qt::UserRole);
        if (serviceItem.isValid())
        {
            return serviceItem;
        }
    }
    return QVariant::fromValue(QString(""));
}

/*!
    \qmlmethod int NetworkSettingsServiceFilter::activeRow()

    Returns the connected service index in the model.
    Returns negative number if no active connection is available.
*/

/*!
    Returns the connected service index in the model.
    Returns negative number if no active connection is available.
*/
int NetworkSettingsServiceFilter::activeRow() const
{
    NetworkSettingsServiceModel* model = qobject_cast<NetworkSettingsServiceModel*>(sourceModel());
    const QList<NetworkSettingsService*> data = model->getModel();
    int row = 0;
    for (NetworkSettingsService* item: data) {
        if (item->type() == m_type &&
                (item->state() == NetworkSettingsState::Ready ||
                 item->state() == NetworkSettingsState::Online)) {
            QModelIndex idx = model->index(row, 0);
            QModelIndex mapped = mapFromSource(idx);
            return mapped.row();
        }
        row++;
    }
    return -1;
}

/*!
    Sets the availability of wired networks to \a wiredNetworksAvailable.
*/
void NetworkSettingsServiceFilter::setWiredNetworksAvailable(bool wiredNetworksAvailable)
{
    m_wiredNetworksAvailable = wiredNetworksAvailable;
    emit wiredNetworksAvailableChanged();
}
