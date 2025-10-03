#include "cpustatmodel.h"
#include "solid_log.h"

CpuStatModel::CpuStatModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &CpuStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &CpuStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &CpuStatModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &CpuStatModel::countInvalidate);

    m_roleNames[CpuRole] ="cpu";
    m_roleNames[NameRole] ="name";
    m_roleNames[UserRole] ="user";
    m_roleNames[NiceRole] ="nice";
    m_roleNames[SystemRole] ="system";
    m_roleNames[IdleRole] ="idle";
    m_roleNames[IOWaitRole] ="iowait";
    m_roleNames[IrqRole] ="irq";
    m_roleNames[SoftIrqRole] ="softirq";
    m_roleNames[StealRole] ="steal";
    m_roleNames[GuestRole] ="guest";
    m_roleNames[GestNiceRole] ="guestNice";
    m_roleNames[TotalRole] ="total";
    m_roleNames[UserTotalRole] ="userTotal";
    m_roleNames[UsageRole] ="usage";

    refresh();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CpuStatModel::refresh);
    timer->setSingleShot(false);

    connect(this, &CpuStatModel::tickChanged, this, [timer](int tick) {
        timer->stop();
        if(tick<=0)
            return;
        timer->setInterval(1000);
        timer->start();
    });
}

QVariant CpuStatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_cpus.size())
        return QVariant();

    const CpuStat cpu = m_cpus.at(index.row());

    switch (role) {
    case CpuRole:
        return QVariant::fromValue<CpuStat>(cpu);
    case NameRole:
        return cpu.name;
    case UserRole:
        return cpu.user;
    case NiceRole:
        return cpu.nice;
    case SystemRole:
        return cpu.system;
    case IdleRole:
        return cpu.idle;
    case IOWaitRole:
        return cpu.iowait;
    case IrqRole:
        return cpu.irq;
    case SoftIrqRole:
        return cpu.softirq;
    case StealRole:
        return cpu.steal;
    case GuestRole:
        return cpu.guest;
    case GestNiceRole:
        return cpu.guestNice;
    case TotalRole:
        return cpu.total;
    case UserTotalRole:
        return cpu.userTotal;
    case UsageRole:
        return cpu.usage;
    }

    return QVariant();
}

int CpuStatModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_cpus.size();
}

QHash<int, QByteArray> CpuStatModel::roleNames() const
{
    return m_roleNames;
}

const CpuStat& CpuStatModel::cpu() const
{
    return m_cpu;
}

const QList<CpuStat>& CpuStatModel::cpus() const
{
    return m_cpus;
}

const QList<quint64>& CpuStatModel::intr() const
{
    return m_intr;
}

quint64 CpuStatModel::ctxt() const
{
    return m_ctxt;
}

quint64 CpuStatModel::btime() const
{
    return m_btime;
}

quint64 CpuStatModel::processes() const
{
    return m_processes;
}

quint64 CpuStatModel::procsRunning() const
{
    return m_procsRunning;
}

quint64 CpuStatModel::procsBlocked() const
{
    return m_procsBlocked;
}

const QList<quint64>& CpuStatModel::softirq() const
{
    return m_softirq;
}

void CpuStatModel::countInvalidate()
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

bool extractCpuStat(CpuStat &value, const QStringList &line)
{
    enum token
    {
        USER       = 0,
        NICE       = 1,
        SYSTEM     = 2,
        IDLE       = 3,
        MIN_COUNT  = 4,
        IOWAIT     = 4,
        IRQ        = 5,
        SOFTIRQ    = 6,
        STEAL      = 7,
        GUEST      = 8,
        GUEST_NICE = 9,
        COUNT
    };

    if(line.size()<MIN_COUNT || line.size()>COUNT)
    {
        SOLIDLOG_WARNING()<<"Corrupted cpu - Unexpected tokens count";
        return false;
    }

    quint64 total_now = 0;
    quint64 user_now = 0;

    for(int i=0; i<line.size(); ++i)
    {
        total_now += line.at(i).toULongLong();
        if(i < IDLE)
            user_now += line.at(i).toULongLong();

        if(i==USER)
            value.user = line.at(i).toULongLong();
        else if(i==NICE)
            value.nice = line.at(i).toULongLong();
        else if(i==SYSTEM)
            value.system = line.at(i).toULongLong();
        else if(i==IDLE)
            value.idle = line.at(i).toULongLong();
        else if(i==IOWAIT)
            value.iowait = line.at(i).toULongLong();
        else if(i==IRQ)
            value.irq = line.at(i).toULongLong();
        else if(i==SOFTIRQ)
            value.softirq = line.at(i).toULongLong();
        else if(i==STEAL)
            value.steal = line.at(i).toULongLong();
        else if(i==GUEST)
            value.guest = line.at(i).toULongLong();
        else if(i==GUEST_NICE)
            value.guestNice = line.at(i).toULongLong();
    }

    // The usage is calcul of user's usage and total usage
    // within a certain period of time.
    quint64 total_over_period = total_now - value.total;
    quint64 user_over_period = user_now - value.userTotal;
    double cpu_usage = static_cast<double>(user_over_period)/total_over_period * 100;

    // Sets the current number of processes
    // to a global value for next iteration.
    value.total = total_now;
    value.userTotal = user_now;
    value.usage = cpu_usage;

    return true;
}

bool extractCpuStat(quint64 &value, const QString &line)
{
    value = line.toULongLong();
    return true;
}

bool extractCpuStat(QList<quint64> &value, const QStringList &line)
{
    quint64 val=0;
    for(const QString& l: line)
    {
        if(!extractCpuStat(val, l))
            return false;
        value.append(val);
    }
    return true;
}

bool CpuStatModel::refresh()
{
    QFile fileStat(QStringLiteral("/proc/stat"));
    if (!fileStat.open(QFile::ReadOnly)) {
        beginResetModel();
        m_cpus.clear();
        endResetModel();
        emit updated();
        return false;
    }

    int cpuCount=0;
    QTextStream fileStream(&fileStat);
    do
    {
        QString line = fileStream.readLine();
        SOLIDLOG_TRACE().noquote()<<line;

        QStringList lineSplit = line.split(" ", Qt::SkipEmptyParts);

        if(lineSplit.isEmpty())
            continue;

        QString keyPart = lineSplit.takeAt(0);

        if(lineSplit.isEmpty())
            continue;

        if(keyPart=="cpu")
        {
            m_cpu.name = keyPart;
            extractCpuStat(m_cpu, lineSplit);
        }
        else if(keyPart.startsWith("cpu"))
        {
            if(cpuCount>=m_cpus.size())
            {
                CpuStat cpu;
                cpu.name = keyPart;
                extractCpuStat(cpu, lineSplit);

                beginInsertRows(QModelIndex(), cpuCount, cpuCount);
                m_cpus.append(cpu);
                endInsertRows();
            }
            else
            {
                extractCpuStat(m_cpus[cpuCount], lineSplit);
                emit this->dataChanged(index(cpuCount), index(cpuCount));
            }
            cpuCount++;
        }
        else if(keyPart=="intr")
            extractCpuStat(m_intr, lineSplit);
        else if(keyPart=="ctxt")
            extractCpuStat(m_ctxt, lineSplit.at(0));
        else if(keyPart=="btime")
            extractCpuStat(m_btime, lineSplit.at(0));
        else if(keyPart=="processes")
            extractCpuStat(m_processes, lineSplit.at(0));
        else if(keyPart=="procs_running")
            extractCpuStat(m_procsRunning, lineSplit.at(0));
        else if(keyPart=="procs_blocked")
            extractCpuStat(m_procsBlocked, lineSplit.at(0));
        else if(keyPart=="softirq")
            extractCpuStat(m_softirq, lineSplit);
    }
    while(!fileStream.atEnd());
    fileStat.close();

    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool CpuStatModel::refresh()
{
    SOLIDLOG_WARNING()<<"CpuStatModel: unsupported platform!";
    return false;
}

#endif
