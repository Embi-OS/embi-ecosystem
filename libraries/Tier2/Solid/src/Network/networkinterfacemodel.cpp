#include "networkinterfacemodel.h"

#define DEV_FILE_PATH (QString("/dev"))

NetworkInterfaceModel::NetworkInterfaceModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &NetworkInterfaceModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &NetworkInterfaceModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &NetworkInterfaceModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &NetworkInterfaceModel::countInvalidate);

    m_roleNames.insert(Qt::UserRole, "entry");
    m_roleNames.insert(Type, "type");
    m_roleNames.insert(Name, "name");
    m_roleNames.insert(Identifier, "identifier");
    m_roleNames.insert(Connected, "connected");
    m_roleNames.insert(Up, "up");
    m_roleNames.insert(Running, "running");
    m_roleNames.insert(Address, "address");
    m_roleNames.insert(MAC, "mac");

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &NetworkInterfaceModel::refresh);
    connect(this, &NetworkInterfaceModel::refreshIntervalChanged, this, [timer](int refreshInterval){
        timer->stop();
        timer->setInterval(refreshInterval);
        if(refreshInterval>0)
            timer->start();
    });

    refresh();
}

QVariant NetworkInterfaceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_interfaces.size())
        return QVariant();

    const QNetworkInterface interface = m_interfaces.at(index.row());

    switch (role) {
    case Qt::UserRole:
        return QVariant::fromValue(interface);
    case Type:
        return (NetworkInterfaceModel::InterfaceType)interface.type();
    case Name:
        return interface.humanReadableName();
    case Identifier:
        return interface.name();
    case Up:
        return interface.flags().testFlags(QNetworkInterface::IsUp);
    case Running:
    case Connected:
        return interface.flags().testFlags(QNetworkInterface::IsRunning);
    case Address: {
        const QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for(const QNetworkAddressEntry& entry : entries) {
            if(entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                return entry.ip().toString();
        }
        return "N/A";
    }
    case MAC:
        return interface.hardwareAddress();
    }

    return QVariant();
}

int NetworkInterfaceModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_interfaces.size();
}

QHash<int, QByteArray> NetworkInterfaceModel::roleNames() const
{
    return m_roleNames;
}

bool NetworkInterfaceModel::contains(const QString& name) const
{
    for(const QNetworkInterface& interface: m_interfaces)
    {
        if(interface.name()==name)
            return true;
    }

    return false;
}

void NetworkInterfaceModel::countInvalidate()
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

void NetworkInterfaceModel::refresh()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    if(m_interfaces.size()!=interfaces.size())
    {
        beginResetModel();
        m_interfaces = std::move(interfaces);
        endResetModel();
    }
    else
    {
        m_interfaces = std::move(interfaces);
        emit this->dataChanged(index(0), index(m_interfaces.size()-1));
    }

    emit updated();
}
