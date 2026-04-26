#include "fstabdrivemodel.h"

FstabDriveModel::FstabDriveModel(QObject* parent) :
    FilesystemDriveModel(parent)
{
    connect(this, &FstabDriveModel::showFsTabDrivesChanged, this, &FilesystemDriveModel::markDirty);
}

void FstabDriveModel::updateStorageList(const QList<QStorageInfo>& storageList)
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
            const QString name = info.fileName().isEmpty() ? info.absoluteFilePath() : info.fileName();
            const QString text = QString("%1 (%2)").arg(name, entry.fsSpec());
            FilesystemDrive* drive = new FilesystemDrive(QStorageInfo(), info, text, this);

            toAppend.append(drive);
        }
    }

    setObjects(toAppend);

    markClean();
}

bool FstabDriveModel::isFstabEntryValid(const FstabEntry& entry)
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
