#include "foldertreemodel.h"
#include "qmodels_log.h"

#include <QStandardPaths>
#ifdef QT_CONCURRENT_LIB
#include <QtConcurrentRun>
#endif
#include <QUtils>

FolderTreeObject::FolderTreeObject(const QString& path, const QFileInfo& info, bool fileIsStandardPath, QObject* parent):
    QTreeObject(parent),
    m_path(path),
    m_fileInfo(info),
    m_fileUrl(QUrl::fromLocalFile(info.absoluteFilePath()).toString()),
    m_fileName(info.fileName().isEmpty() ? info.absoluteFilePath() : info.fileName()),
    m_filePath(info.absoluteFilePath()),
    m_fileBaseName(info.baseName()),
    m_fileCompleteBaseName(info.completeBaseName()),
    m_fileSuffix(info.suffix()),
    m_fileCompleteSuffix(info.completeSuffix()),
    m_fileSize(info.size()),
    m_filePermissions(info.permissions()),
    m_fileAccessed(info.lastRead()),
    m_fileModified(info.lastModified()),
    m_fileIsDir(info.isDir()),
    m_fileIsFile(info.isFile()),
    m_fileIsLink(info.isSymLink()),
    m_fileIsStandardPath(fileIsStandardPath)
{
    updateName();
    updateText();
    updateInfo();
    updateGroup();
}

FolderTreeObject::FolderTreeObject(const QString& path, const QStorageInfo& info, QObject* parent):
    FolderTreeObject(path, QFileInfo(path), false, parent)
{
    m_fileIsDrive = true;
    m_driveInfo = info;
    m_driveBlockSize = info.blockSize();
    m_driveBytesAvailable = info.bytesAvailable();
    m_driveBytesFree = info.bytesFree();
    m_driveBytesTotal = info.bytesTotal();
    m_driveDevice = info.device();
    m_driveDisplayName = info.displayName();
    m_driveFileSystemType = info.fileSystemType();
    m_driveIsReadOnly = info.isReadOnly();
    m_driveIsReady = info.isReady();
    m_driveIsRoot = info.isRoot();
    m_driveIsBoot = info.rootPath().startsWith("/boot") || info.name().toLower().contains("boot");
    m_driveIsConfig = info.rootPath().startsWith("/config") || info.name().toLower().contains("config");
    m_driveIsOverlay = info.fileSystemType().contains("overlay");
    m_driveIsNetwork = info.fileSystemType().contains("nfs") || info.fileSystemType().contains("smb") || info.fileSystemType().contains("cifs");
    m_driveIsUsb = info.fileSystemType().contains("exfat") || info.fileSystemType().contains("vfat") || info.fileSystemType().contains("ntfs");
    m_driveIsValid = info.isValid();
    m_driveName = info.name();
    m_driveRootPath = info.rootPath();
    m_driveSubVolume = info.subvolume();

    if(m_driveIsRoot || m_driveIsRoot || m_driveIsConfig || m_driveIsOverlay)
        m_driveDisplayType = tr("Système");
    else if(m_driveIsNetwork)
        m_driveDisplayType = tr("Réseau");
    else if(m_driveIsUsb)
        m_driveDisplayType = tr("USB");
    else
        m_driveDisplayType = tr("Standard");

    updateName();
    updateText();
    updateInfo();
    updateGroup();
}

void FolderTreeObject::updateName()
{
    if(m_fileIsDrive) {
        m_name = QString("%1").arg(m_driveName.isEmpty() ? m_fileName : m_driveName);
        return;
    } else if(m_fileIsDir) {
        if(m_fileName==":/")
            m_name = "qrc:/";
        else
            m_name = m_fileName;
        return;
    } else if(m_fileIsFile) {
        m_name = m_fileName;
        return;
    } else if(m_fileIsLink) {
        m_name = m_fileName;
        return;
    }
    m_name = m_fileName;
}

void FolderTreeObject::updateText()
{
    if(m_fileIsDrive) {
        m_text = QString("%1 (%2)").arg(m_driveName.isEmpty() ? m_fileName : m_driveName, m_driveDevice);
        return;
    } else if(m_fileIsDir) {
        m_text = m_fileName;
        return;
    } else if(m_fileIsFile) {
        m_text = m_fileName;
        return;
    } else if(m_fileIsLink) {
        m_text = m_fileName;
        return;
    }
    m_text = m_fileName;
}

void FolderTreeObject::updateInfo()
{
    if(m_fileIsDrive) {
        if(m_driveBytesTotal == 0)
            m_info = "";
        else
            m_info = QString("%1 / %2 (%3%)")
                         .arg(bytes((qint64) (m_driveBytesTotal-m_driveBytesAvailable)), bytes((qint64) m_driveBytesTotal))
                         .arg((m_driveBytesTotal-m_driveBytesAvailable)*100/m_driveBytesTotal);
        return;
    } else if(m_fileIsDir) {
        m_info = "";
        return;
    } else if(m_fileIsFile) {
        m_info = bytes(m_fileSize);
        return;
    } else if(m_fileIsLink) {
        m_info = "";
        return;
    }
    m_info = "";
}

void FolderTreeObject::updateGroup()
{
    if(m_fileIsDrive) {
        m_group = "100_drives";
        return;
    } else if(m_fileIsStandardPath) {
        m_group = "300_standards";
        return;
    }
    m_group = "200_paths";
}

FolderTreeModel::FolderTreeModel(QObject* parent):
    QObjectTreeModel(parent, &FolderTreeObject::staticMetaObject),
    m_watcher(new QFileSystemWatcher(this)),
    m_refreshCaller(this)
{
    connect(this, &FolderTreeModel::enabledChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::pathChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::additionalPathsChanged, this, &FolderTreeModel::queueSelect);

    connect(this, &FolderTreeModel::nameFiltersChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::caseSensitiveChanged, this, &FolderTreeModel::queueSelect);

    connect(this, &FolderTreeModel::showRootDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showSnapPackageDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showUnmountedAutofsDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showTmpfsDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showOverlayDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showBootDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showConfigDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showReadOnlyDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showQrcDrivesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showStandardPathsChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showApplicationDirPathChanged, this, &FolderTreeModel::queueSelect);

    connect(this, &FolderTreeModel::showDirsChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showEmptyDirsChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showFilesChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showDirsFirstChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showDotChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showDotDotChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showDotAndDotDotChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showHiddenChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::showOnlyReadableChanged, this, &FolderTreeModel::queueSelect);

    connect(this, &FolderTreeModel::sortCaseSensitiveChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::sortReversedChanged, this, &FolderTreeModel::queueSelect);
    connect(this, &FolderTreeModel::sortFieldChanged, this, &FolderTreeModel::queueSelect);

    m_refreshCaller.setSingleShot(true);
    connect(&m_refreshCaller, &QTimer::timeout, this, &FolderTreeModel::queueSelect);

    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &FolderTreeModel::markDirty);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FolderTreeModel::markDirty);
}

void FolderTreeModel::classBegin()
{

}

void FolderTreeModel::componentComplete()
{
    QObjectTreeModel::componentComplete();
    select();
}

bool FolderTreeModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return !isEmpty();
    if (parent.column() > 0)
        return false;
    FolderTreeObject* childItem = static_cast<FolderTreeObject*>(parent.internalPointer());
    return childItem->getFileIsDir() || childItem->size()>0;
}
bool FolderTreeModel::canFetchMore(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return false;
    FolderTreeObject* childItem = static_cast<FolderTreeObject*>(parent.internalPointer());
    return !childItem->getPopulated();
}
void FolderTreeModel::fetchMore(const QModelIndex& parent)
{
    if(!parent.isValid())
        return;
    FolderTreeObject* childItem = static_cast<FolderTreeObject*>(parent.internalPointer());
    if(childItem->getPopulated())
        return;
    const QList<QTreeObject*> objects = fetchPath(childItem->getPath(), filters(), sortFlags(), m_nameFilters);
    insertObjects(objects, childItem);
    childItem->setPopulated(true);
}
Qt::ItemFlags FolderTreeModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QObjectTreeModel::flags(index);
    if (!index.isValid())
        return flags;

    if(index.column() == 0) {
        FolderTreeObject* childItem = static_cast<FolderTreeObject*>(index.internalPointer());
        if (childItem->isEmpty() && !childItem->getFileIsDir())
            flags |= Qt::ItemNeverHasChildren;
    }

    return flags;
}

QDir::Filters FolderTreeModel::filters() const
{
    QDir::Filters filters;

    if(m_caseSensitive)
        filters.setFlag(QDir::CaseSensitive);
    if(m_showDirs)
        filters.setFlag(QDir::Dirs);
    if(m_showHidden)
        filters.setFlag(QDir::Hidden);
    if(m_showOnlyReadable)
        filters.setFlag(QDir::Readable);
    if(!m_showDot)
        filters.setFlag(QDir::NoDot);
    if(!m_showDotDot)
        filters.setFlag(QDir::NoDotDot);
    if(m_showFiles)
        filters.setFlag(QDir::Files);

    return filters;
}
QDir::SortFlags FolderTreeModel::sortFlags() const
{
    QDir::SortFlags sortFlags;

    if(m_showDirsFirst)
        sortFlags.setFlag(QDir::DirsFirst);
    if(m_sortReversed)
        sortFlags.setFlag(QDir::Reversed);
    if(!m_sortCaseSensitive)
        sortFlags.setFlag(QDir::IgnoreCase);
    switch(m_sortField)
    {
    case FolderTreeModelSortFields::Name:
        sortFlags.setFlag(QDir::Name);
        break;
    case FolderTreeModelSortFields::Time:
        sortFlags.setFlag(QDir::Time);
        break;
    case FolderTreeModelSortFields::Size:
        sortFlags.setFlag(QDir::Size);
        break;
    case FolderTreeModelSortFields::Type:
        sortFlags.setFlag(QDir::Type);
        break;
    default:
        break;
    }

    return sortFlags;
}
FolderTreeModel::DriveFilters FolderTreeModel::driveFilters() const
{
    FolderTreeModel::DriveFilters filters;

    if(m_showSnapPackageDrives)
        filters.setFlag(FolderTreeModel::SnapPackage);
    if(m_showUnmountedAutofsDrives)
        filters.setFlag(FolderTreeModel::UnmountedAutofs);
    if(m_showTmpfsDrives)
        filters.setFlag(FolderTreeModel::Tmpfs);
    if(m_showOverlayDrives)
        filters.setFlag(FolderTreeModel::Overlay);
    if(m_showConfigDrives)
        filters.setFlag(FolderTreeModel::Config);
    if(m_showBootDrives)
        filters.setFlag(FolderTreeModel::Boot);
    if(m_showRootDrives)
        filters.setFlag(FolderTreeModel::Root);
    if(m_showReadOnlyDrives)
        filters.setFlag(FolderTreeModel::ReadOnly);

    return filters;
}

QList<QStorageInfo> FolderTreeModel::mountedVolumes(bool mount)
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

void FolderTreeModel::queueSelect()
{
    if(!m_completed || !m_enabled)
        return;

    if (m_delayed) {
        if (!m_selectQueued) {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &FolderTreeModel::select, Qt::QueuedConnection);
        }
    } else {
        select();
    }
}

void FolderTreeModel::select()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    m_selectQueued = false;

    if(!m_enabled || m_loading)
        return;

    if(!m_watcher->files().isEmpty())
        m_watcher->removePaths(m_watcher->files());
    if(!m_watcher->directories().isEmpty())
        m_watcher->removePaths(m_watcher->directories());

    if(m_path.isEmpty())
    {
        setLoading(true);

#ifdef QT_CONCURRENT_LIB
        auto future = QtConcurrent::run([]() {
            return FolderTreeModel::mountedVolumes(true);
        });
        future.then(this, [this](const QList<QStorageInfo>& storageList) mutable {
            setStorageList(storageList);
            setLoading(false);
        });
#else
        const QList<QStorageInfo> storageList = FolderTreeModel::mountedVolumes(true);
        setStorageList(storageList);
        setLoading(false);
#endif
    }
    else
    {
        const QList<QTreeObject*> objects = fetchPath(m_path, filters(), sortFlags(), m_nameFilters);
        setObjects(objects);

        m_watcher->addPath(m_path);
    }
}

void FolderTreeModel::setStorageList(const QList<QStorageInfo>& storageList)
{
    QList<QTreeObject*> objects;
    objects += fetchDrives(storageList, driveFilters());

    if(m_showQrcDrives)
        objects += fetchQrcDrives();

    if(m_showApplicationDirPath)
        objects += fetchApplicationDirPath();

    if(m_showStandardPaths)
        objects += fetchStandardPaths();

    objects.reserve(objects.size() + m_additionalPaths.size());
    for(const QString& path: std::as_const(m_additionalPaths))
    {
        QFileInfo info = QFileInfo(path);
        if(info.exists() && info.isDir()) {
            objects.append(new FolderTreeObject(info.absoluteFilePath(), info));
        }
        else
            QMODELSLOG_WARNING()<<"Can't fetch additional path:"<<path;
    }

    setObjects(objects);

    m_watcher->addPath("/dev/");
    m_watcher->addPath("/mnt/");
    m_watcher->addPath("/media/");
}

void FolderTreeModel::markDirty(const QString &path)
{
    m_refreshCaller.stop();
    m_refreshCaller.start(500);
}

QList<QTreeObject*> FolderTreeModel::fetchPath(const QString &path, QDir::Filters filters, QDir::SortFlags sortFlags, const QStringList& nameFilters)
{
    QFileInfo info = QFileInfo(path);
    if(!info.exists() || !info.isDir())
    {
        QMODELSLOG_WARNING()<<"Can't fetch file:"<<path;
        return QList<QTreeObject*>();
    }

    const QDir dir = QDir(path);

    // 1. Récupérer les dossiers uniquement (sans appliquer les nameFilters)
    const QFileInfoList folderList = dir.entryInfoList(filters & ~QDir::Files, sortFlags);

    // 2. Récupérer les fichiers uniquement (avec le nameFilters)
    const QFileInfoList fileList = dir.entryInfoList(nameFilters, filters & ~QDir::Dirs & ~QDir::NoDotAndDotDot, sortFlags);

    QList<QTreeObject*> toAppend;
    toAppend.reserve(folderList.size() + fileList.size());

    for(const QFileInfo& fileInfo: folderList) {
        toAppend.append(new FolderTreeObject(fileInfo.absoluteFilePath(), fileInfo));
    }
    for(const QFileInfo& fileInfo: fileList) {
        toAppend.append(new FolderTreeObject(fileInfo.absoluteFilePath(), fileInfo));
    }

    return toAppend;
}

QList<QTreeObject*> FolderTreeModel::fetchDrives(const QList<QStorageInfo>& storageList, const DriveFilters filters)
{
    QList<QStorageInfo> tmp;
    for(const QStorageInfo& storage: storageList)
    {
        if(!(storage.isValid() && storage.isReady()))
            continue;

        // isSnapPackage
        if((storage.rootPath().startsWith("/snap") || storage.fileSystemType()==("squashfs"))
            && !filters.testFlag(FolderTreeModel::SnapPackage))
            continue;

        // isUnmountedAutofs
        if((storage.fileSystemType()==("autofs") || storage.bytesTotal()==0)
            && !filters.testFlag(FolderTreeModel::UnmountedAutofs))
            continue;

        // isTmpfs
        if((storage.fileSystemType()==("tmpfs"))
            && !filters.testFlag(FolderTreeModel::Tmpfs))
            continue;

        // isOverlay
        if((storage.fileSystemType()==("overlay"))
            && !filters.testFlag(FolderTreeModel::Overlay))
            continue;

        // isConfig
        if((storage.rootPath().startsWith("/config") || storage.name().toLower().contains("config"))
            && !filters.testFlag(FolderTreeModel::Config))
            continue;

        // isBoot
        if((storage.rootPath().startsWith("/boot") || storage.name().toLower().contains("boot"))
            && !filters.testFlag(FolderTreeModel::Boot))
            continue;

        // isRoot
        if((storage.isRoot())
            && !filters.testFlag(FolderTreeModel::Root))
            continue;

        // isReadOnly
        if((storage.isReadOnly())
            && !filters.testFlag(FolderTreeModel::ReadOnly))
            continue;

        tmp.append(storage);
    }

    QList<QTreeObject*> toAppend;
    toAppend.reserve(tmp.size());
    for(const QStorageInfo& storage: tmp) {
        toAppend.append(new FolderTreeObject(storage.rootPath(), storage));
    }

    return toAppend;
}

QList<QTreeObject*> FolderTreeModel::fetchQrcDrives()
{
    QList<QTreeObject*> toAppend;
    toAppend.reserve(1);

    const QString path = ":/";
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path)));
    }

    return toAppend;
}

QList<QTreeObject*> FolderTreeModel::fetchStandardPaths()
{
    QList<QTreeObject*> toAppend;

    QString path;

    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    path = QString("%1/.local/share/Trash").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path), true));
    }

    return toAppend;
}

QList<QTreeObject*> FolderTreeModel::fetchApplicationDirPath()
{
    QList<QTreeObject*> toAppend;
    toAppend.reserve(1);

    const QString path = QCoreApplication::applicationDirPath();
    if (QFile::exists(path)) {
        toAppend.append(new FolderTreeObject(path, QFileInfo(path)));
    }

    return toAppend;
}
