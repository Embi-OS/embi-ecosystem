#include "netstatmodel.h"
#include "solid_log.h"

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

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
/*****************************************************************************
 * GNU/Linux
 ****************************************************************************/

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
        SOLIDLOG_WARNING()<<"Corrupted net device line - Wrong number of tokens";
        return false;
    }

    value.interface = line.at(INTERFACE);
    value.interface.remove(1, value.interface.size()-1); // Remove ':';

    quint64 rxBytes = value.rxBytes;
    value.rxBytes = line.at(RX_BYTES).toULongLong();
    value.rxPackets = line.at(RX_PACKETS).toULongLong();
    value.rxErrs = line.at(RX_ERRS).toULongLong();
    value.rxDrop = line.at(RX_DROP).toULongLong();
    value.rxFifo = line.at(RX_FIFO).toULongLong();
    value.rxFrame = line.at(RX_FRAME).toULongLong();
    value.rxCompressed = line.at(RX_COMPRESSED).toULongLong();
    value.rxMulticast = line.at(RX_MULTICAST).toULongLong();

    qint64 rxDelta = value.rxBytes-rxBytes;
    if(mSecSinceLast!=0)
        value.rxThroughput = (static_cast<double>(rxDelta)/mSecSinceLast)*1000;

    quint64 txBytes = value.txBytes;
    value.txBytes = line.at(TX_BYTES).toULongLong();
    value.txPackets = line.at(TX_PACKETS).toULongLong();
    value.txErrs = line.at(TX_ERRS).toULongLong();
    value.txDrop = line.at(TX_DROP).toULongLong();
    value.txFifo = line.at(TX_FIFO).toULongLong();
    value.txColls = line.at(TX_COLLS).toULongLong();
    value.txCarrier = line.at(TX_CARRIER).toULongLong();
    value.txCompressed = line.at(TX_COMPRESSED).toULongLong();

    qint64 txDelta = value.txBytes-txBytes;
    if(mSecSinceLast!=0)
        value.txThroughput = (static_cast<double>(txDelta)/mSecSinceLast)*1000;

    return true;
}

bool NetStatModel::refresh()
{
    beginResetModel();

    QFile fileStat(QStringLiteral("/proc/net/dev"));
    if (!fileStat.open(QFile::ReadOnly)) {
        emit updated();
        endResetModel();
        return false;
    }

    int netCount=0;
    QTextStream fileStream(&fileStat);
    do
    {
        QString line = fileStream.readLine();
        SOLIDLOG_TRACE().noquote()<<line;

        if(!line.contains(":"))
            continue;

        QStringList lineSplit = line.split(" ");
        lineSplit.removeAll("");

        if(netCount>=m_nets.size())
        {
            NetStat net;
            m_nets.append(net);
        }
        extractNetStat(m_nets[netCount], lineSplit, m_time.elapsed());
        netCount++;
    }
    while(!fileStream.atEnd());
    fileStat.close();

    NetStat net;
    net.interface = "NET";
    for(const NetStat& n: std::as_const(m_nets))
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
    m_net = net;

    m_time.restart();
    emit updated();
    endResetModel();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool NetStatModel::refresh()
{
    SOLIDLOG_WARNING()<<"NetStatModel: unsupported platform!";
    return false;
}

#endif
