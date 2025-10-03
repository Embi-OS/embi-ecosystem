#include "memoryinfo.h"
#include "solid_log.h"

// clang-format off
#if defined(Q_OS_WINDOWS)
    #include <Windows.h>    // Windows.h must stay above Pspapi.h
    #include <Psapi.h>
#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    #include <QByteArray>
    #include <QFile>
        #include <QByteArrayView>
#elif defined(Q_OS_MACOS)
    #include <mach/mach.h>
    #include <sys/sysctl.h>
#elif defined(Q_OS_FREEBSD)
    #include <fcntl.h>
    #include <kvm.h>
    #include <sys/sysctl.h>
#endif
// clang-format on

MemoryInfo::MemoryInfo(QObject *parent) :
    QObject(parent)
{
    refresh();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MemoryInfo::refresh);
    timer->setSingleShot(false);

    connect(this, &MemoryInfo::tickChanged, this, [timer](int tick) {
        timer->stop();
        if(tick<=0)
            return;
        timer->setInterval(1000);
        timer->start();
    });
}

bool MemoryInfo::isNull() const
{
    return m_totalPhysical == 0;
}

quint64 MemoryInfo::totalPhysical() const
{
    return m_totalPhysical;
}

quint64 MemoryInfo::freePhysical() const
{
    return m_freePhysical;
}

quint64 MemoryInfo::availablePhysical() const
{
    return m_availablePhysical;
}

quint64 MemoryInfo::cached() const
{
    return m_cached;
}

quint64 MemoryInfo::buffers() const
{
    return m_buffers;
}

quint64 MemoryInfo::totalSwapFile() const
{
    return m_totalSwapFile;
}

quint64 MemoryInfo::freeSwapFile() const
{
    return m_freeSwapFile;
}

float MemoryInfo::usage() const
{
    if(isNull())
        return 0;
    return float((double(m_totalPhysical-m_availablePhysical)/double(m_totalPhysical))*100.);
}

#if defined(Q_OS_WINDOWS)
/*****************************************************************************
 * Windows
 ****************************************************************************/

struct SwapInfo {
    quint64 totalPageFilePages = 0;
    quint64 freePageFilePages = 0;
};

BOOL __stdcall pageInfo(LPVOID pContext, PENUM_PAGE_FILE_INFORMATION pPageFileInfo, LPCWSTR lpFilename)
{
    Q_UNUSED(lpFilename)
    if (auto sw = static_cast<SwapInfo *>(pContext)) {
        sw->totalPageFilePages += pPageFileInfo->TotalSize;
        sw->freePageFilePages += (pPageFileInfo->TotalSize - pPageFileInfo->TotalInUse);
        return true;
    }
    return false;
}

bool MemoryInfo::refresh()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (!GlobalMemoryStatusEx(&statex)) {
        return false;
    }

    PERFORMANCE_INFORMATION pi;
    DWORD pisz = sizeof(pi);
    if (!GetPerformanceInfo(&pi, pisz)) {
        return false;
    }

    SwapInfo si;
    if (!EnumPageFiles(pageInfo, &si)) {
        return false;
    }

    m_totalPhysical = statex.ullTotalPhys;
    m_availablePhysical = statex.ullAvailPhys;
    m_freePhysical = statex.ullAvailPhys;
    m_totalSwapFile = si.totalPageFilePages * pi.PageSize;
    m_freeSwapFile = si.freePageFilePages * pi.PageSize;
    m_cached = pi.SystemCache * pi.PageSize;
    m_buffers = 0;

    emit updated();

    return true;
}

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
/*****************************************************************************
 * GNU/Linux
 ****************************************************************************/

bool extractBytes(quint64 &value, const QByteArray &buffer, const QByteArrayView &beginPattern, qsizetype &from)
{
    QByteArrayView endPattern("kB");
    auto beginIdx = buffer.indexOf(beginPattern, from);
    if (beginIdx > -1) {
        auto start = beginIdx + beginPattern.size();
        auto endIdx = buffer.indexOf(endPattern, start);
        if (endIdx > -1) {
            from = endIdx + endPattern.size();
            auto ok = false;
            value = buffer.mid(start, endIdx - start).toULongLong(&ok) * 1024;
            return ok;
        }
    }
    if (from) { // Wrong order? Restart from the beginning
        SOLIDLOG_WARNING()<<"MemoryInfo: extractBytes: wrong order when extracting"<<beginPattern;
        from = 0;
        return extractBytes(value, buffer, beginPattern, from);
    }
    return false;
}

bool MemoryInfo::refresh()
{
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QByteArray meminfo = file.readAll();
    file.close();

    qsizetype miFrom = 0;
    quint64 totalPhys = 0;
    if (!extractBytes(totalPhys, meminfo, "MemTotal:", miFrom)) {
        return false;
    }
    quint64 freePhys = 0;
    if (!extractBytes(freePhys, meminfo, "MemFree:", miFrom)) {
        return false;
    }
    quint64 availPhys = 0;
    if (!extractBytes(availPhys, meminfo, "MemAvailable:", miFrom)) {
        return false;
    }
    quint64 buffers = 0;
    if (!extractBytes(buffers, meminfo, "Buffers:", miFrom)) {
        return false;
    }
    quint64 cached = 0;
    if (!extractBytes(cached, meminfo, "Cached:", miFrom)) {
        return false;
    }
    quint64 swapTotal = 0;
    if (!extractBytes(swapTotal, meminfo, "SwapTotal:", miFrom)) {
        return false;
    }
    quint64 swapFree = 0;
    if (!extractBytes(swapFree, meminfo, "SwapFree:", miFrom)) {
        return false;
    }
    quint64 sharedMem = 0;
    if (!extractBytes(sharedMem, meminfo, "Shmem:", miFrom)) {
        return false;
    }
    quint64 sReclaimable = 0;
    if (!extractBytes(sReclaimable, meminfo, "SReclaimable:", miFrom)) {
        return false;
    }

    // Source HTOP: https://github.com/htop-dev/htop/blob/main/linux/LinuxProcessList.c
    m_totalPhysical = totalPhys;
    // NOTE: another viable solution: m_availablePhysical = std::min(availPhys, totalPhys - (committedAs - cached - (swapTotal - swapFree)))
    m_availablePhysical = availPhys ? std::min(availPhys, totalPhys) : freePhys;
    m_freePhysical = freePhys;
    m_totalSwapFile = swapTotal;
    m_freeSwapFile = swapFree;
    m_cached = cached + sReclaimable - sharedMem;
    m_buffers = buffers;

    emit updated();

    return true;
}

#elif defined(Q_OS_MACOS)
/*****************************************************************************
 * macOS
 ****************************************************************************/

template<class T>
bool sysctlread(const char *name, T &var)
{
    auto sz = sizeof(var);
    return (sysctlbyname(name, &var, &sz, NULL, 0) == 0);
}

bool MemoryInfo::refresh()
{
    quint64 memSize = 0;
    quint64 pageSize = 0;
    xsw_usage swapUsage;

    int mib[2];
    size_t sz = 0;

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    sz = sizeof(memSize);
    if (sysctl(mib, 2, &memSize, &sz, NULL, 0) != KERN_SUCCESS) {
        return false;
    }

    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;
    sz = sizeof(pageSize);
    if (sysctl(mib, 2, &pageSize, &sz, NULL, 0) != KERN_SUCCESS) {
        return false;
    }

    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    sz = sizeof(swapUsage);
    if (sysctl(mib, 2, &swapUsage, &sz, NULL, 0) != KERN_SUCCESS) {
        return false;
    }

    quint64 zfs_arcstats_size = 0;
    if (!sysctlread("kstat.zfs.misc.arcstats.size", zfs_arcstats_size)) {
        zfs_arcstats_size = 0; // no ZFS used
    }

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    vm_statistics64_data_t vmstat;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmstat, &count) != KERN_SUCCESS) {
        return false;
    }

    m_totalPhysical = memSize;
    m_availablePhysical = memSize - (vmstat.internal_page_count + vmstat.compressor_page_count + vmstat.wire_count) * pageSize;
    m_freePhysical = vmstat.free_count * pageSize;
    m_totalSwapFile = swapUsage.xsu_total;
    m_freeSwapFile = swapUsage.xsu_avail;
    m_cached = vmstat.external_page_count * pageSize + zfs_arcstats_size;
    m_buffers = 0;

    emit updated();

    return true;
}

#elif defined(Q_OS_FREEBSD)
/*****************************************************************************
 * FreeBSD
 ****************************************************************************/

template<class T>
bool sysctlread(const char *name, T &var)
{
    auto sz = sizeof(var);
    return (sysctlbyname(name, &var, &sz, NULL, 0) == 0);
}

bool MemoryInfo::refresh()
{
    quint64 memSize = 0;
    quint64 pageSize = 0;

    int mib[4];
    size_t sz = 0;

    mib[0] = CTL_HW;
    mib[1] = HW_PHYSMEM;
    sz = sizeof(memSize);
    if (sysctl(mib, 2, &memSize, &sz, NULL, 0) != 0) {
        return false;
    }

    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;
    sz = sizeof(pageSize);
    if (sysctl(mib, 2, &pageSize, &sz, NULL, 0) != 0) {
        return false;
    }

    quint32 v_pageSize = 0;
    if (sysctlread("vm.stats.vm.v_page_size", v_pageSize)) {
        pageSize = v_pageSize;
    }
    quint64 zfs_arcstats_size = 0;
    if (!sysctlread("kstat.zfs.misc.arcstats.size", zfs_arcstats_size)) {
        zfs_arcstats_size = 0; // no ZFS used
    }
    quint32 v_cache_count = 0;
    if (!sysctlread("vm.stats.vm.v_cache_count", v_cache_count)) {
        return false;
    }
    quint32 v_inactive_count = 0;
    if (!sysctlread("vm.stats.vm.v_inactive_count", v_inactive_count)) {
        return false;
    }
    quint32 v_free_count = 0;
    if (!sysctlread("vm.stats.vm.v_free_count", v_free_count)) {
        return false;
    }
    quint64 vfs_bufspace = 0;
    if (!sysctlread("vfs.bufspace", vfs_bufspace)) {
        return false;
    }

    quint64 swap_tot = 0;
    quint64 swap_free = 0;
    if (auto kd = kvm_open("/dev/null", "/dev/null", "/dev/null", O_RDONLY, "kvm_open")) {
        struct kvm_swap swap;
        // if you specify a maxswap value of 1, the function will typically return the
        // value 0 and the single kvm_swap structure will be filled with the grand total over all swap devices.
        auto nswap = kvm_getswapinfo(kd, &swap, 1, 0);
        if (nswap == 0) {
            swap_tot = swap.ksw_total;
            swap_free = swap.ksw_used;
        }
        swap_free = (swap_tot - swap_free) * pageSize;
        swap_tot *= pageSize;
    }

    // Source HTOP: https://github.com/htop-dev/htop/blob/main/freebsd/FreeBSDProcessList.c
    m_totalPhysical = memSize;
    m_availablePhysical = pageSize * (v_cache_count + v_free_count + v_inactive_count) + vfs_bufspace + zfs_arcstats_size;
    m_freePhysical = pageSize * v_free_count;
    m_totalSwapFile = swap_tot;
    m_freeSwapFile = swap_free;
    m_cached = pageSize * v_cache_count + zfs_arcstats_size;
    m_buffers = vfs_bufspace;

    emit updated();

    return true;
}

#else
/*****************************************************************************
 * Unsupported platform
 ****************************************************************************/

bool MemoryInfo::refresh()
{
    SOLIDLOG_WARNING()<<"MemoryInfo: unsupported platform!";
    return false;
}

#endif
