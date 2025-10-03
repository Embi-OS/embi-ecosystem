#include "filesystemdrivemodel.h"
#include "files_log.h"

#include "fstab.h"

#include <QUtils>

#ifdef QT_CONCURRENT_LIB
#include <QtConcurrentRun>
#endif

#define DEV_FILE_PATH (QString("/dev"))
#define MNT_FILE_PATH (QString("/mnt"))
#define MEDIA_FILE_PATH (QString("/media"))
FilesystemDrive::FilesystemDrive(const QStorageInfo& storage, const QFileInfo& info, QObject* parent) :
    QObject (parent)
{
    m_driveInfo = storage;
    m_driveBlockSize = storage.blockSize();
    m_driveBytesAvailable = storage.bytesAvailable();
    m_driveBytesFree = storage.bytesFree();
    m_driveBytesTotal = storage.bytesTotal();
    m_driveDevice = storage.device();
    m_driveDisplayName = storage.displayName();
    m_driveFileSystemType = storage.fileSystemType();
    m_driveIsReadOnly = storage.isReadOnly();
    m_driveIsReady = storage.isReady();
    m_driveIsRoot = storage.isRoot();
    m_driveIsBoot = storage.rootPath().startsWith("/boot") || storage.name().toLower().contains("boot");
    m_driveIsConfig = storage.rootPath().startsWith("/config") || storage.name().toLower().contains("config");
    m_driveIsOverlay = storage.fileSystemType().contains("overlay");
    m_driveIsNetwork = storage.fileSystemType().contains("nfs") || storage.fileSystemType().contains("smb") || storage.fileSystemType().contains("cifs");
    m_driveIsUsb = storage.fileSystemType().contains("exfat") || storage.fileSystemType().contains("vfat") || storage.fileSystemType().contains("ntfs");
    m_driveIsValid = storage.isValid();
    m_driveIsMounted = storage.isValid() && storage.isReady() && storage.rootPath()==info.absoluteFilePath();
    m_driveName = storage.name();
    m_driveRootPath = storage.rootPath();
    m_driveSubVolume = storage.subvolume();

    if(m_driveIsRoot || m_driveIsRoot || m_driveIsConfig || m_driveIsOverlay)
        m_driveDisplayType = tr("Système");
    else if(m_driveIsNetwork)
        m_driveDisplayType = tr("Réseau");
    else if(m_driveIsUsb)
        m_driveDisplayType = tr("USB");
    else
        m_driveDisplayType = tr("Standard");

    m_fileInfo = info;
    m_fileUrl = QUrl::fromLocalFile(info.absoluteFilePath()).toString();
    m_fileName = info.fileName().isEmpty() ? info.absoluteFilePath() : info.fileName();
    m_filePath = info.absoluteFilePath();
    m_fileBaseName = info.baseName();
    m_fileCompleteBaseName = info.completeBaseName();
    m_fileSuffix = info.suffix();
    m_fileCompleteSuffix = info.completeSuffix();
    m_fileSize = info.size();
    m_filePermissions = info.permissions();
    m_fileAccessed = info.lastRead();
    m_fileModified = info.lastModified();

    m_name = m_driveName.isEmpty() ? m_fileName : m_driveName;
    m_text = QString("%1 (%2)").arg(m_name, m_driveDevice);
    m_info = QString("%1 / %2 (%3%)")
                 .arg(bytes((qint64) (m_driveBytesTotal-m_driveBytesAvailable)), bytes((qint64) m_driveBytesTotal))
                 .arg((m_driveBytesTotal-m_driveBytesAvailable)*100/m_driveBytesTotal);
};

QString FilesystemDrive::toString() const
{
    QString str;

    str.append("Filesystem Drive:\n");
    str.append(QString("Name            : %1\n").arg(m_name));
    str.append(QString("File name       : %1\n").arg(m_fileName));
    str.append(QString("Drive name      : %1\n").arg(m_driveName));
    str.append(QString("Display name    : %1\n").arg(m_driveDisplayName));
    str.append(QString("Device          : %1\n").arg(QString::fromLocal8Bit(m_driveDevice)));
    str.append(QString("Path            : %1\n").arg(m_driveRootPath));
    str.append(QString("isReadOnly      : %1\n").arg(m_driveIsReadOnly));
    str.append(QString("fileSystemType  : %1\n").arg(QString::fromLocal8Bit(m_driveFileSystemType)));
    str.append(QString("Infos           : %1").arg(m_info));

    return str;
};

FilesystemDriveModel::FilesystemDriveModel(QObject* parent) :
    QObjectListModel(parent, &FilesystemDrive::staticMetaObject),
    m_watcher(new QFileSystemWatcher(this)),
    m_refreshCaller(this)
{
    m_refreshCaller.setSingleShot(true);
    connect(&m_refreshCaller, &QTimer::timeout, this, &FilesystemDriveModel::refresh);

    refresh();

    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FilesystemDriveModel::markDirty);
    m_watcher->addPath(DEV_FILE_PATH);
    m_watcher->addPath(MNT_FILE_PATH);
    m_watcher->addPath(MEDIA_FILE_PATH);

    connect(this, &FilesystemDriveModel::showRootDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showSnapPackageDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showUnmountedAutofsDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showTmpfsDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showBootDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showConfigDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showReadOnlyDrivesChanged, this, &FilesystemDriveModel::markDirty);
    connect(this, &FilesystemDriveModel::showFsTabDrivesChanged, this, &FilesystemDriveModel::markDirty);
}

QList<QStorageInfo> FilesystemDriveModel::mountedVolumes(bool mount)
{
#if QT_CONFIG(process)
    if(mount)
    {
        QProcess process;
        process.start("mount", {"-a"});
        process.waitForFinished();
    }
#endif
    return QStorageInfo::mountedVolumes();
}

void FilesystemDriveModel::refresh()
{
    if(m_isRefreshing)
        return;
    m_isRefreshing = true;

#ifdef QT_CONCURRENT_LIB
    auto future = QtConcurrent::run([]() {
        return FilesystemDriveModel::mountedVolumes(true);
    });
    future.then(this, [this](const QList<QStorageInfo>& storageList) {
        setStorageList(storageList);
        m_isRefreshing = false;
    });
#else
    const QList<QStorageInfo> storageList = FilesystemDriveModel::mountedVolumes(true);
    setStorageList(storageList);
    m_isRefreshing = false;
#endif
}

void FilesystemDriveModel::setStorageList(const QList<QStorageInfo>& storageList)
{
    QList<QStorageInfo> storages;
    storages.reserve(storageList.size());
    for(const QStorageInfo& storage: storageList)
    {
        if(isStorageInfoValid(storage))
            storages.append(storage);
    }

    QList<QObject*> toAppend;
    toAppend.reserve(storages.size());
    for(const QStorageInfo& storage: storages)
    {
        FilesystemDrive* drive = new FilesystemDrive(storage, QFileInfo(storage.rootPath()));
        toAppend.append(drive);
    }

    if(m_showFsTabDrives)
    {
        const QList<FstabEntry> fsTabEntries = FstabEntry::entries();
        toAppend.reserve(toAppend.size() + fsTabEntries.size());
        for(const FstabEntry& entry: fsTabEntries)
        {
            if(entry.entryType()==FstabEntryTypes::Comment)
                continue;
            const QStorageInfo storage = QStorageInfo(entry.mountPoint());
            if(isStorageInfoValid(storage) && entry.mountPoint()==storage.rootPath()) {
                if(storages.contains(storage))
                    continue;
                FilesystemDrive* drive = new FilesystemDrive(storage, QFileInfo(storage.rootPath()));
                toAppend.append(drive);
                continue;
            }

            if(!isFstabEntryValid(entry))
                continue;

            const QFileInfo info = QFileInfo(entry.mountPoint());
            FilesystemDrive* drive = new FilesystemDrive(QStorageInfo(), info, this);
            drive->m_text = QString("%1 (%2)").arg(drive->getName(), entry.fsSpec());

            toAppend.append(drive);
        }
    }

    setObjects(toAppend);

    markClean();
}

void FilesystemDriveModel::eject(FilesystemDrive* drive)
{
#if QT_CONFIG(process)
    if(!drive)
        return;

    if(drive->getDriveIsRoot() || drive->getDriveIsBoot())
    {
        FILESLOG_CRITICAL()<<"Cannot eject boot or root drive"<<drive->getText();
    }

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        markDirty();
        proc->deleteLater();
    });
    proc->start("umount", {drive->getDriveRootPath()});
#else
    FILESLOG_CRITICAL()<<"Cannot eject drive"<<drive->getText();
#endif
}

void FilesystemDriveModel::markDirty()
{
    m_refreshCaller.stop();
    m_refreshCaller.start(500);
    setProcessing(true);
}

void FilesystemDriveModel::markClean()
{
    m_refreshCaller.stop();
    setProcessing(false);
}

bool FilesystemDriveModel::isStorageInfoValid(const QStorageInfo& storage)
{
    if(!(storage.isValid() && storage.isReady()))
        return false;

    // isSnapPackage
    if((storage.rootPath().startsWith("/snap") || storage.fileSystemType().toLower()==("squashfs"))
            && !getShowSnapPackageDrives())
        return false;

    // isUnmountedAutofs
    if((storage.fileSystemType().toLower()==("autofs") || storage.bytesTotal()==0)
            && !getShowUnmountedAutofsDrives())
        return false;

    // isTmpfs
    if((storage.fileSystemType().toLower()==("tmpfs"))
        && !getShowTmpfsDrives())
        return false;

    // isOverlay
    if((storage.fileSystemType().toLower()==("overlay"))
        && !getShowOverlayDrives())
        return false;

    // isConfig
    if((storage.rootPath().startsWith("/config") || storage.name().toLower().contains("config"))
        && !getShowConfigDrives())
        return false;

    // isBoot
    if((storage.rootPath().startsWith("/boot") || storage.name().toLower().contains("boot"))
        && !getShowBootDrives())
        return false;

    // isRoot
    if((storage.isRoot())
            && !getShowRootDrives())
        return false;

    // isReadOnly
    if((storage.isReadOnly())
            && !getShowReadOnlyDrives())
        return false;

    return true;
}

bool FilesystemDriveModel::isFstabEntryValid(const FstabEntry& entry)
{
    const QStorageInfo storage = QStorageInfo(entry.mountPoint());

    if(storage.isValid() && storage.rootPath()==entry.mountPoint())
        return isStorageInfoValid(storage);

    // isUnmountedAutofs
    if((entry.type().toLower()==("auto"))
        && !getShowUnmountedAutofsDrives())
        return false;

    // isTmpfs
    if((entry.type().toLower()==("tmpfs"))
        && !getShowTmpfsDrives())
        return false;

    if((entry.mountPoint().toLower()==("none")) || (entry.type().toLower()==("swap")))
        return false;

    return true;
}
