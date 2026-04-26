#include "netstatmodel.h"
#include "fluid_log.h"

#include <algorithm>

#if defined(Q_OS_WINDOWS)
#include <QLibrary>
#include <winsock2.h>
#include <netioapi.h>
#ifdef interface
#undef interface
#endif
#endif

NetStatModel::NetStatModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &NetStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &NetStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &NetStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &NetStatModel::countInvalidate);

    m_roleNames[NetRole] ="net";

    refresh();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &NetStatModel::refresh);
    timer->setSingleShot(false);

    connect(this, &NetStatModel::tickChanged, this, [timer](int tick) {
        timer->stop();
        if(tick<=0)
            return;
        timer->setInterval(1000);
        timer->start();
    });
}

QVariant NetStatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_nets.size())
        return QVariant();

    const NetStat net = m_nets.at(index.row());

    switch (role) {
    case NetRole:
        return QVariant::fromValue<NetStat>(net);
    case InterfaceRole:
        return net.interface;
    case RxBytesRole:
        return net.rxBytes;
    case RxPacketsRole:
        return net.rxPackets;
    case RxErrsRole:
        return net.rxErrs;
    case RxDropRole:
        return net.rxDrop;
    case RxFifoRole:
        return net.rxFifo;
    case RxFrameRole:
        return net.rxFrame;
    case RxCompressedRole:
        return net.rxCompressed;
    case RxMulticastRole:
        return net.rxMulticast;
    case RxThroughputRole:
        return net.rxThroughput;
    case TxBytesRole:
        return net.txBytes;
    case TxPacketsRole:
        return net.txPackets;
    case TxErrsRole:
        return net.txErrs;
    case TxDropRole:
        return net.txDrop;
    case TxFifoRole:
        return net.txFifo;
    case TxCollsRole:
        return net.txColls;
    case TxCarrierRole:
        return net.txCarrier;
    case TxCompressedRole:
        return net.txCompressed;
    case TxThroughputRole:
        return net.txThroughput;
    }

    return QVariant();
}

int NetStatModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_nets.size();
}

QHash<int, QByteArray> NetStatModel::roleNames() const
{
    return m_roleNames;
}

const NetStat& NetStatModel::net() const
{
    return m_net;
}

const QList<NetStat>& NetStatModel::nets() const
{
    return m_nets;
}

void NetStatModel::countInvalidate()
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

void updateNetThroughput(NetStat& value, quint64 previousRxBytes, quint64 previousTxBytes, int mSecSinceLast)
{
    if(mSecSinceLast<=0)
        return;

    const quint64 rxDelta = value.rxBytes>=previousRxBytes ? value.rxBytes-previousRxBytes : 0;
    value.rxThroughput = (static_cast<double>(rxDelta)/mSecSinceLast)*1000;

    const quint64 txDelta = value.txBytes>=previousTxBytes ? value.txBytes-previousTxBytes : 0;
    value.txThroughput = (static_cast<double>(txDelta)/mSecSinceLast)*1000;
}

NetStat totalNetStat(const QList<NetStat>& nets)
{
    NetStat net;
    net.interface = "NET";
    for(const NetStat& n: nets)
    {
        net.rxBytes += n.rxBytes;
        net.rxPackets += n.rxPackets;
        net.rxErrs += n.rxErrs;
        net.rxDrop += n.rxDrop;
        net.rxFifo += n.rxFifo;
        net.rxFrame += n.rxFrame;
        net.rxCompressed += n.rxCompressed;
        net.rxMulticast += n.rxMulticast;
        net.rxThroughput += n.rxThroughput;
        net.txBytes += n.txBytes;
        net.txPackets += n.txPackets;
        net.txErrs += n.txErrs;
        net.txDrop += n.txDrop;
        net.txFifo += n.txFifo;
        net.txColls += n.txColls;
        net.txCarrier += n.txCarrier;
        net.txCompressed += n.txCompressed;
        net.txThroughput += n.txThroughput;
    }
    return net;
}

#if defined(Q_OS_WINDOWS)
/*****************************************************************************
 * Windows
 ****************************************************************************/

using GetIfTable2Function = NETIO_STATUS (NETIOAPI_API_ *)(PMIB_IF_TABLE2*);
using FreeMibTableFunction = VOID (NETIOAPI_API_ *)(PVOID);

struct NetIoApi
{
    GetIfTable2Function getIfTable2 = nullptr;
    FreeMibTableFunction freeMibTable = nullptr;

    bool isValid() const
    {
        return getIfTable2 && freeMibTable;
    }
};

NetIoApi resolveNetIoApi()
{
    static QLibrary iphlpapi(QStringLiteral("iphlpapi"));
    if(!iphlpapi.isLoaded() && !iphlpapi.load())
    {
        FLUIDLOG_WARNING()<<"NetStatModel: unable to load iphlpapi";
        return {};
    }

    return {
        reinterpret_cast<GetIfTable2Function>(iphlpapi.resolve("GetIfTable2")),
        reinterpret_cast<FreeMibTableFunction>(iphlpapi.resolve("FreeMibTable")),
    };
}

QString interfaceName(const MIB_IF_ROW2& row)
{
    QString alias = QString::fromWCharArray(row.Alias).trimmed();
    if(!alias.isEmpty())
        return alias;

    QString description = QString::fromWCharArray(row.Description).trimmed();
    if(!description.isEmpty())
        return description;

    return QStringLiteral("if%1").arg(row.InterfaceIndex);
}

bool NetStatModel::refresh()
{
    const NetIoApi netIoApi = resolveNetIoApi();
    if(!netIoApi.isValid())
    {
        if (!m_nets.isEmpty()) {
            beginResetModel();
            m_nets.clear();
            endResetModel();
        }
        m_net = NetStat();
        emit updated();
        return false;
    }

    PMIB_IF_TABLE2 table = nullptr;
    const NETIO_STATUS status = netIoApi.getIfTable2(&table);
    if(status != NO_ERROR)
    {
        if (!m_nets.isEmpty()) {
            beginResetModel();
            m_nets.clear();
            endResetModel();
        }
        m_net = NetStat();
        emit updated();
        return false;
    }

    const int elapsed = m_time.isValid() ? m_time.elapsed() : 0;
    QHash<QString, NetStat> previousNets;
    for(const NetStat& net: std::as_const(m_nets))
        previousNets.insert(net.interface, net);

    QList<NetStat> nets;
    nets.reserve(table->NumEntries);

    for(ULONG i=0; i<table->NumEntries; ++i)
    {
        const MIB_IF_ROW2& row = table->Table[i];

        NetStat net;
        net.interface = interfaceName(row);
        net.rxBytes = row.InOctets;
        net.rxPackets = row.InUcastPkts + row.InNUcastPkts;
        net.rxErrs = row.InErrors;
        net.rxDrop = row.InDiscards;
        net.txBytes = row.OutOctets;
        net.txPackets = row.OutUcastPkts + row.OutNUcastPkts;
        net.txErrs = row.OutErrors;
        net.txDrop = row.OutDiscards;

        const NetStat previous = previousNets.value(net.interface);
        updateNetThroughput(net, previous.rxBytes, previous.txBytes, elapsed);

        nets.append(net);
    }

    netIoApi.freeMibTable(table);

    beginResetModel();
    m_nets = std::move(nets);
    m_net = totalNetStat(m_nets);
    m_time.restart();
    emit updated();
    endResetModel();

    return true;
}

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
/*****************************************************************************
 * GNU/Linux
 ****************************************************************************/

bool extractNetStat(quint64& value, const QString& line)
{
    bool ok = false;
    const quint64 parsed = line.toULongLong(&ok);
    if(!ok)
        return false;

    value = parsed;
    return true;
}

bool extractNetStat(NetStat &value, const QStringList &line, int mSecSinceLast=0)
{
    enum token
    {
        INTERFACE     = 0,
        RX_BYTES      = 1,
        RX_PACKETS    = 2,
        RX_ERRS       = 3,
        RX_DROP       = 4,
        RX_FIFO       = 5,
        RX_FRAME      = 6,
        RX_COMPRESSED = 7,
        RX_MULTICAST  = 8,
        TX_BYTES      = 9,
        TX_PACKETS    = 10,
        TX_ERRS       = 11,
        TX_DROP       = 12,
        TX_FIFO       = 13,
        TX_COLLS      = 14,
        TX_CARRIER    = 15,
        TX_COMPRESSED = 16,
        COUNT
    };

    if(line.size()!=COUNT)
    {
        FLUIDLOG_WARNING()<<"Corrupted net device line - Wrong number of tokens";
        return false;
    }

    value.interface = line.at(INTERFACE);
    if(value.interface.endsWith(":"))
        value.interface.chop(1);

    quint64 rxBytes = value.rxBytes;
    if(!extractNetStat(value.rxBytes, line.at(RX_BYTES)) ||
       !extractNetStat(value.rxPackets, line.at(RX_PACKETS)) ||
       !extractNetStat(value.rxErrs, line.at(RX_ERRS)) ||
       !extractNetStat(value.rxDrop, line.at(RX_DROP)) ||
       !extractNetStat(value.rxFifo, line.at(RX_FIFO)) ||
       !extractNetStat(value.rxFrame, line.at(RX_FRAME)) ||
       !extractNetStat(value.rxCompressed, line.at(RX_COMPRESSED)) ||
       !extractNetStat(value.rxMulticast, line.at(RX_MULTICAST)))
    {
        FLUIDLOG_WARNING()<<"Corrupted net device line - Invalid rx counter";
        return false;
    }

    quint64 txBytes = value.txBytes;
    if(!extractNetStat(value.txBytes, line.at(TX_BYTES)) ||
       !extractNetStat(value.txPackets, line.at(TX_PACKETS)) ||
       !extractNetStat(value.txErrs, line.at(TX_ERRS)) ||
       !extractNetStat(value.txDrop, line.at(TX_DROP)) ||
       !extractNetStat(value.txFifo, line.at(TX_FIFO)) ||
       !extractNetStat(value.txColls, line.at(TX_COLLS)) ||
       !extractNetStat(value.txCarrier, line.at(TX_CARRIER)) ||
       !extractNetStat(value.txCompressed, line.at(TX_COMPRESSED)))
    {
        FLUIDLOG_WARNING()<<"Corrupted net device line - Invalid tx counter";
        return false;
    }

    updateNetThroughput(value, rxBytes, txBytes, mSecSinceLast);

    return true;
}

bool NetStatModel::refresh()
{
    QFile fileStat(QStringLiteral("/proc/net/dev"));
    if (!fileStat.open(QFile::ReadOnly)) {
        if (!m_nets.isEmpty()) {
            beginResetModel();
            m_nets.clear();
            endResetModel();
        }
        m_net = NetStat();
        emit updated();
        return false;
    }

    const int elapsed = m_time.isValid() ? m_time.elapsed() : 0;
    QHash<QString, NetStat> previousNets;
    for(const NetStat& net: std::as_const(m_nets))
        previousNets.insert(net.interface, net);

    QList<NetStat> nets;
    QTextStream fileStream(&fileStat);
    do
    {
        QString line = fileStream.readLine();
        FLUIDLOG_TRACE().noquote()<<line;

        if(!line.contains(":"))
            continue;

        QStringList lineSplit = line.split(" ");
        lineSplit.removeAll("");

        QString interface = lineSplit.isEmpty() ? QString() : lineSplit.first();
        if(interface.endsWith(":"))
            interface.chop(1);
        NetStat net = previousNets.value(interface);
        if(extractNetStat(net, lineSplit, elapsed))
            nets.append(net);
    }
    while(!fileStream.atEnd());
    fileStat.close();

    for(int row=m_nets.size()-1; row>=0; --row)
    {
        const QString interface = m_nets.at(row).interface;
        const auto it = std::find_if(nets.cbegin(), nets.cend(), [&interface](const NetStat& net) {
            return net.interface == interface;
        });

        if(it != nets.cend())
            continue;

        beginRemoveRows(QModelIndex(), row, row);
        m_nets.removeAt(row);
        endRemoveRows();
    }

    for(int row=0; row<nets.size(); ++row)
    {
        if(row<m_nets.size())
        {
            m_nets[row] = nets.at(row);
            emit this->dataChanged(index(row), index(row));
            continue;
        }

        beginInsertRows(QModelIndex(), row, row);
        m_nets.append(nets.at(row));
        endInsertRows();
    }

    m_net = totalNetStat(m_nets);

    m_time.restart();
    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool NetStatModel::refresh()
{
    if (!m_nets.isEmpty()) {
        beginResetModel();
        m_nets.clear();
        endResetModel();
    }
    m_net = NetStat();
    emit updated();
    return false;
}

#endif
