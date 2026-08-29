#ifndef FILESYSTEMDRIVEMODEL_H
#define FILESYSTEMDRIVEMODEL_H

#include <QModels>
#include <QDefs>
#include <QStorageInfo>

#include "filesystemmountwatcher.h"

QString filesystemDriveTypesDisplayText(const int value);
Q_ENUM_CLASS_I18N(FilesystemDriveTypes, FilesystemDriveType, filesystemDriveTypesDisplayText,
                  System,
                  Network,
                  Usb,
                  Standard)

class FilesystemDriveModel;
class FilesystemDrive : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_REF_PROPERTY(QStorageInfo, driveInfo, {})
    Q_CONSTANT_VAR_PROPERTY(int, driveBlockSize, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesAvailable, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesFree, 0)
    Q_CONSTANT_VAR_PROPERTY(qint64, driveBytesTotal, 0)
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveDevice, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveDisplayName, "")
    Q_CONSTANT_VAR_PROPERTY(FilesystemDriveTypes::Enum, driveType, FilesystemDriveTypes::Standard)
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveFileSystemType, "")
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsReadOnly, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsReady, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsRoot, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsBoot, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsConfig, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsOverlay, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsNetwork, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsUsb, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsValid, false)
    Q_CONSTANT_VAR_PROPERTY(bool, driveIsMounted, false)
    Q_CONSTANT_REF_PROPERTY(QString, driveName, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveRootPath, "")
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveSubVolume, "")

    Q_CONSTANT_REF_PROPERTY(QFileInfo, fileInfo, {})
    Q_CONSTANT_REF_PROPERTY(QString, fileUrl, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileName, "")
    Q_CONSTANT_REF_PROPERTY(QString, filePath, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileBaseName, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileCompleteBaseName, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileSuffix, "")
    Q_CONSTANT_REF_PROPERTY(QString, fileCompleteSuffix, "")
    Q_CONSTANT_VAR_PROPERTY(qint64, fileSize, 0)
    Q_CONSTANT_VAR_PROPERTY(int, filePermissions, 0)
    Q_CONSTANT_REF_PROPERTY(QDateTime, fileAccessed, {})
    Q_CONSTANT_REF_PROPERTY(QDateTime, fileModified, {})

    Q_CONSTANT_REF_PROPERTY(QString, name, "")
    Q_CONSTANT_REF_PROPERTY(QString, text, "")
    Q_CONSTANT_REF_PROPERTY(QString, info, "")

    friend FilesystemDriveModel;
public:
    explicit FilesystemDrive(const QStorageInfo& storage, const QFileInfo& info, QObject* parent=nullptr);
    explicit FilesystemDrive(const QStorageInfo& storage, const QFileInfo& info, const QString& text, QObject* parent=nullptr);

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

    Q_PROPERTY(bool canEject READ canEject CONSTANT FINAL)

public:
    explicit FilesystemDriveModel(QObject* parent = nullptr);

    static bool canEject();
    static QList<QStorageInfo> mountedVolumes(bool mount=false);

public slots:
    void refresh();
    void eject(FilesystemDrive* drive);

    void markDirty();

signals:
    void watcherMountAdded(const QString &mountPoint, const QString &device);
    void watcherMountRemoved(const QString &mountPoint, const QString &device);

protected slots:
    virtual void updateStorageList(const QList<QStorageInfo>& storageList);
    void markClean();

protected:
    bool isStorageInfoValid(const QStorageInfo& storage);

    FileSystemMountWatcher* m_mountWatcher=nullptr;
    QTimer m_refreshCaller;
    bool m_isRefreshing=false;
};

#endif // FILESYSTEMDRIVEMODEL_H
