#include "cpustatmodel.h"
#include "fluid_log.h"

#if defined(Q_OS_WINDOWS)
#include <QLibrary>
#include <QThread>
#include <Windows.h>
#endif

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

void updateCpuUsage(CpuStat& value, quint64 totalNow, quint64 userNow)
{
    const quint64 totalOverPeriod = totalNow>=value.total ? totalNow-value.total : 0;
    const quint64 userOverPeriod = userNow>=value.userTotal ? userNow-value.userTotal : 0;

    value.total = totalNow;
    value.userTotal = userNow;
    value.usage = totalOverPeriod>0 ? static_cast<double>(userOverPeriod)/totalOverPeriod * 100 : 0;
}

#if defined(Q_OS_WINDOWS)
/*****************************************************************************
 * Windows
 ****************************************************************************/

constexpr ULONG SystemProcessorPerformanceInformation = 8;

struct WindowsProcessorPerformanceInformation
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
};

using NtQuerySystemInformationFunction = LONG (WINAPI *)(ULONG, PVOID, ULONG, PULONG);

struct NtApi
{
    NtQuerySystemInformationFunction querySystemInformation = nullptr;

    bool isValid() const
    {
        return querySystemInformation;
    }
};

NtApi resolveNtApi()
{
    static QLibrary ntdll(QStringLiteral("ntdll"));
    if(!ntdll.isLoaded() && !ntdll.load())
    {
        FLUIDLOG_WARNING()<<"CpuStatModel: unable to load ntdll";
        return {};
    }

    return {
        reinterpret_cast<NtQuerySystemInformationFunction>(ntdll.resolve("NtQuerySystemInformation")),
    };
}

quint64 largeIntegerToU64(const LARGE_INTEGER& value)
{
    return value.QuadPart>0 ? static_cast<quint64>(value.QuadPart) : 0;
}

bool queryProcessorPerformanceInformation(QList<WindowsProcessorPerformanceInformation>& infos)
{
    const NtApi ntApi = resolveNtApi();
    if(!ntApi.isValid())
        return false;

    int processorCount = qMax(1, QThread::idealThreadCount());
    QByteArray buffer;

    for(int attempt=0; attempt<4; ++attempt)
    {
        buffer.resize(processorCount * int(sizeof(WindowsProcessorPerformanceInformation)));

        ULONG returnLength = 0;
        const LONG status = ntApi.querySystemInformation(SystemProcessorPerformanceInformation,
                                                         buffer.data(),
                                                         static_cast<ULONG>(buffer.size()),
                                                         &returnLength);
        if(status>=0)
        {
            const int byteCount = returnLength>0 ? int(returnLength) : buffer.size();
            const int infoCount = byteCount / int(sizeof(WindowsProcessorPerformanceInformation));
            const auto* data = reinterpret_cast<const WindowsProcessorPerformanceInformation*>(buffer.constData());

            infos.clear();
            infos.reserve(infoCount);
            for(int i=0; i<infoCount; ++i)
                infos.append(data[i]);

            return !infos.isEmpty();
        }

        if(returnLength>static_cast<ULONG>(buffer.size()))
            processorCount = int(returnLength / sizeof(WindowsProcessorPerformanceInformation)) + 1;
        else
            processorCount *= 2;
    }

    return false;
}

void extractCpuStat(CpuStat& value, const WindowsProcessorPerformanceInformation& info)
{
    const quint64 idle = largeIntegerToU64(info.IdleTime);
    const quint64 kernel = largeIntegerToU64(info.KernelTime);
    const quint64 user = largeIntegerToU64(info.UserTime);
    const quint64 dpc = largeIntegerToU64(info.DpcTime);
    const quint64 interrupt = largeIntegerToU64(info.InterruptTime);
    const quint64 system = kernel>idle+dpc+interrupt ? kernel-idle-dpc-interrupt : 0;

    value.user = user;
    value.nice = 0;
    value.system = system;
    value.idle = idle;
    value.iowait = 0;
    value.irq = interrupt;
    value.softirq = dpc;
    value.steal = 0;
    value.guest = 0;
    value.guestNice = 0;

    const quint64 userNow = value.user + value.nice + value.system + value.irq + value.softirq;
    const quint64 totalNow = userNow + value.idle + value.iowait + value.steal + value.guest + value.guestNice;
    updateCpuUsage(value, totalNow, userNow);
}

bool CpuStatModel::refresh()
{
    QList<WindowsProcessorPerformanceInformation> infos;
    if(!queryProcessorPerformanceInformation(infos))
    {
        if (!m_cpus.isEmpty()) {
            beginResetModel();
            m_cpus.clear();
            endResetModel();
        }
        m_cpu = CpuStat();
        m_intr.clear();
        m_ctxt = 0;
        m_btime = 0;
        m_processes = 0;
        m_procsRunning = 0;
        m_procsBlocked = 0;
        m_softirq.clear();
        emit updated();
        return false;
    }

    QList<CpuStat> cpus;
    cpus.reserve(infos.size());

    CpuStat totalCpu = m_cpu;
    totalCpu.name = QStringLiteral("cpu");
    totalCpu.user = 0;
    totalCpu.nice = 0;
    totalCpu.system = 0;
    totalCpu.idle = 0;
    totalCpu.iowait = 0;
    totalCpu.irq = 0;
    totalCpu.softirq = 0;
    totalCpu.steal = 0;
    totalCpu.guest = 0;
    totalCpu.guestNice = 0;

    m_intr.clear();
    m_softirq.clear();

    for(int i=0; i<infos.size(); ++i)
    {
        CpuStat cpu = i<m_cpus.size() ? m_cpus.at(i) : CpuStat();
        cpu.name = QStringLiteral("cpu%1").arg(i);
        extractCpuStat(cpu, infos.at(i));
        cpus.append(cpu);

        totalCpu.user += cpu.user;
        totalCpu.system += cpu.system;
        totalCpu.idle += cpu.idle;
        totalCpu.irq += cpu.irq;
        totalCpu.softirq += cpu.softirq;

        m_intr.append(infos.at(i).InterruptCount);
        m_softirq.append(cpu.softirq);
    }

    const quint64 userNow = totalCpu.user + totalCpu.nice + totalCpu.system + totalCpu.irq + totalCpu.softirq;
    const quint64 totalNow = userNow + totalCpu.idle + totalCpu.iowait + totalCpu.steal + totalCpu.guest + totalCpu.guestNice;
    updateCpuUsage(totalCpu, totalNow, userNow);

    const int commonCpuCount = qMin(m_cpus.size(), cpus.size());
    for(int i=0; i<commonCpuCount; ++i)
    {
        m_cpus[i] = cpus.at(i);
        emit this->dataChanged(index(i), index(i));
    }

    if(cpus.size()>m_cpus.size())
    {
        const int first = m_cpus.size();
        const int last = cpus.size()-1;
        beginInsertRows(QModelIndex(), first, last);
        for(int i=first; i<cpus.size(); ++i)
            m_cpus.append(cpus.at(i));
        endInsertRows();
    }
    else if(cpus.size()<m_cpus.size())
    {
        beginRemoveRows(QModelIndex(), cpus.size(), m_cpus.size()-1);
        while(m_cpus.size()>cpus.size())
            m_cpus.removeLast();
        endRemoveRows();
    }

    m_cpu = totalCpu;
    m_ctxt = 0;
    m_btime = 0;
    m_processes = 0;
    m_procsRunning = 0;
    m_procsBlocked = 0;

    emit updated();
    return true;
}

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
/*****************************************************************************
 * GNU/Linux
 ****************************************************************************/

bool extractCpuStat(quint64 &value, const QString &line)
{
    bool ok = false;
    const quint64 parsed = line.toULongLong(&ok);
    if(!ok)
        return false;

    value = parsed;
    return true;
}

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
        FLUIDLOG_WARNING()<<"Corrupted cpu - Unexpected tokens count";
        return false;
    }

    quint64 total_now = 0;
    quint64 user_now = 0;
    QList<quint64> values;
    values.reserve(line.size());

    for(int i=0; i<line.size(); ++i)
    {
        quint64 parsedValue = 0;
        if(!extractCpuStat(parsedValue, line.at(i)))
        {
            FLUIDLOG_WARNING()<<"Corrupted cpu - Invalid numeric token";
            return false;
        }

        values.append(parsedValue);
    }

    for(int i=0; i<values.size(); ++i)
    {
        const quint64 parsedValue = values.at(i);

        total_now += parsedValue;
        if(i < IDLE)
            user_now += parsedValue;

        if(i==USER)
            value.user = parsedValue;
        else if(i==NICE)
            value.nice = parsedValue;
        else if(i==SYSTEM)
            value.system = parsedValue;
        else if(i==IDLE)
            value.idle = parsedValue;
        else if(i==IOWAIT)
            value.iowait = parsedValue;
        else if(i==IRQ)
            value.irq = parsedValue;
        else if(i==SOFTIRQ)
            value.softirq = parsedValue;
        else if(i==STEAL)
            value.steal = parsedValue;
        else if(i==GUEST)
            value.guest = parsedValue;
        else if(i==GUEST_NICE)
            value.guestNice = parsedValue;
    }

    updateCpuUsage(value, total_now, user_now);

    return true;
}

bool extractCpuStatList(QList<quint64> &value, const QStringList &line)
{
    QList<quint64> parsedValues;
    parsedValues.reserve(line.size());

    for(const QString& l: line)
    {
        quint64 val=0;
        if(!extractCpuStat(val, l))
            return false;
        parsedValues.append(val);
    }

    value = std::move(parsedValues);
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
    QList<quint64> intr;
    QList<quint64> softirq;
    QTextStream fileStream(&fileStat);
    do
    {
        QString line = fileStream.readLine();
        FLUIDLOG_TRACE().noquote()<<line;

        QStringList lineSplit = line.split(" ", Qt::SkipEmptyParts);

        if(lineSplit.isEmpty())
            continue;

        QString keyPart = lineSplit.takeAt(0);

        if(lineSplit.isEmpty())
            continue;

        if(keyPart=="cpu")
        {
            CpuStat cpu = m_cpu;
            cpu.name = keyPart;
            if(extractCpuStat(cpu, lineSplit))
                m_cpu = cpu;
            else
            {
                m_cpu = CpuStat();
                m_cpu.name = keyPart;
            }
        }
        else if(keyPart.startsWith("cpu"))
        {
            CpuStat cpu = cpuCount<m_cpus.size() ? m_cpus.at(cpuCount) : CpuStat();
            cpu.name = keyPart;
            if(!extractCpuStat(cpu, lineSplit))
                continue;

            if(cpuCount>=m_cpus.size())
            {
                beginInsertRows(QModelIndex(), cpuCount, cpuCount);
                m_cpus.append(cpu);
                endInsertRows();
            }
            else
            {
                m_cpus[cpuCount] = cpu;
                emit this->dataChanged(index(cpuCount), index(cpuCount));
            }
            cpuCount++;
        }
        else if(keyPart=="intr")
            extractCpuStatList(intr, lineSplit);
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
            extractCpuStatList(softirq, lineSplit);
    }
    while(!fileStream.atEnd());
    fileStat.close();

    if(cpuCount<m_cpus.size())
    {
        beginRemoveRows(QModelIndex(), cpuCount, m_cpus.size()-1);
        while(m_cpus.size()>cpuCount)
            m_cpus.removeLast();
        endRemoveRows();
    }

    m_intr = std::move(intr);
    m_softirq = std::move(softirq);

    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool CpuStatModel::refresh()
{
    if (!m_cpus.isEmpty()) {
        beginResetModel();
        m_cpus.clear();
        endResetModel();
    }
    m_cpu = CpuStat();
    m_intr.clear();
    m_ctxt = 0;
    m_btime = 0;
    m_processes = 0;
    m_procsRunning = 0;
    m_procsBlocked = 0;
    m_softirq.clear();
    emit updated();
    return false;
}

#endif
