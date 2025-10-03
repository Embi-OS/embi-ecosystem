#ifndef FILESYSTEMDRIVEMODEL_H
#define FILESYSTEMDRIVEMODEL_H

#include <QModels>
#include <QDefs>
#include <QStorageInfo>
#include <QFileSystemWatcher>

struct FstabEntry;
class FilesystemDriveModel;
class FilesystemDrive : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_REF_PROPERTY(QStorageInfo, driveInfo, DriveInfo, {})
    Q_CONSTANT_VAR_PROPERTY(int, driveBlockSize, DriveBlockSize, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesAvailable, DriveBytesAvailable, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesFree, DriveBytesFree, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesTotal, DriveBytesTotal, 0)
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveDevice, DriveDevice, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveDisplayName, DriveDisplayName, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveDisplayType, DriveDisplayType, "")
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveFileSystemType, DriveFileSystemType, "")
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsReadOnly, DriveIsReadOnly, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsReady, DriveIsReady, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsRoot, DriveIsRoot, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsBoot, DriveIsBoot, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsConfig, DriveIsConfig, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsOverlay, DriveIsOverlay, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsNetwork, DriveIsNetwork, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsUsb, DriveIsUsb, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsValid, DriveIsValid, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsMounted, DriveIsMounted, false)
    Q_CONSTANT_REF_PROPERTY(QString, driveName, DriveName, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveRootPath, DriveRootPath, "")
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveSubVolume, DriveSubVolume, "")

    Q_CONSTANT_REF_PROPERTY(QFileInfo, fileInfo, FileInfo, {})
    Q_CONSTANT_REF_PROPERTY(QString, fileUrl, FileUrl, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileName, FileName, "")
    Q_CONSTANT_REF_PROPERTY(QString, filePath, FilePath, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileBaseName, FileBaseName, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileCompleteBaseName, FileCompleteBaseName, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileSuffix, FileSuffix, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileCompleteSuffix, FileCompleteSuffix, "")
    Q_CONSTANT_VAR_PROPERTY(qint64, fileSize, FileSize, 0)
    Q_CONSTANT_VAR_PROPERTY(int, filePermissions, FilePermissions, 0)
    Q_CONSTANT_REF_PROPERTY(QDateTime, fileAccessed, FileAccessed, {})
    Q_CONSTANT_REF_PROPERTY(QDateTime, fileModified, FileModified, {})

    Q_CONSTANT_REF_PROPERTY(QString, name, Name, "")
    Q_CONSTANT_REF_PROPERTY(QString, text, Text, "")
    Q_CONSTANT_REF_PROPERTY(QString, info, Info, "")

    friend FilesystemDriveModel;
public:
    explicit FilesystemDrive(const QStorageInfo& storage, const QFileInfo& info, QObject* parent=nullptr);

    Q_INVOKABLE QString toString() const;
};

class FilesystemDriveModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_VAR_PROPERTY(bool, processing, Processing, false)

    Q_WRITABLE_VAR_PROPERTY(bool, showRootDrives, ShowRootDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showSnapPackageDrives, ShowSnapPackageDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showUnmountedAutofsDrives, ShowUnmountedAutofsDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showTmpfsDrives, ShowTmpfsDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showOverlayDrives, ShowOverlayDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showBootDrives, ShowBootDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showConfigDrives, ShowConfigDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showReadOnlyDrives, ShowReadOnlyDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showFsTabDrives, ShowFsTabDrives, false)

public:
    explicit FilesystemDriveModel(QObject* parent = nullptr);

    static QList<QStorageInfo> mountedVolumes(bool mount=false);

public slots:
    void refresh();
    void setStorageList(const QList<QStorageInfo>& storageList);
    void eject(FilesystemDrive* drive);

    void markDirty();

private slots:
    void markClean();

private:
    bool isStorageInfoValid(const QStorageInfo& storage);
    bool isFstabEntryValid(const FstabEntry& entry);

    QFileSystemWatcher* m_watcher=nullptr;
    QTimer m_refreshCaller;
    bool m_isRefreshing=false;
};

#endif // FILESYSTEMDRIVEMODEL_H
