#ifndef FOLDERTREEMODEL_H
#define FOLDERTREEMODEL_H

#include <QDefs>
#include <QFileInfo>
#include <QDateTime>
#include <QStorageInfo>
#include <QFileSystemWatcher>

#include "qobjecttreemodel.h"

Q_ENUM_CLASS(FolderTreeTypes, FolderTreeType,
             All,
             File,
             Dir)

Q_ENUM_CLASS(FolderTreeModelSortFields, FolderTreeModelSortField,
             Unsorted, // no sorting is applied
             Name, // sort by filename
             Time, // sort by time modified
             Size, // sort by file size
             Type) // sort by file type(extension)

class FolderTreeObject: public QTreeObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Use FolderTreeModel")

    Q_WRITABLE_VAR_PROPERTY(bool, populated, Populated, false)
    Q_CONSTANT_REF_PROPERTY(QString, path, Path, "")

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
    Q_CONSTANT_REF_PROPERTY(QString, driveName, DriveName, "")
    Q_CONSTANT_REF_PROPERTY(QString, driveRootPath, DriveRootPath, "")
    Q_CONSTANT_REF_PROPERTY(QByteArray, driveSubVolume, DriveSubVolume, "")

    Q_CONSTANT_VAR_PROPERTY(bool, fileIsDrive, FileIsDrive, false)
    Q_PROPERTY(bool isDrive READ getFileIsDrive CONSTANT FINAL)

    Q_CONSTANT_VAR_PROPERTY(bool, fileIsDir, FileIsDir, false)
    Q_PROPERTY(bool isDir READ getFileIsDir CONSTANT FINAL)

    Q_CONSTANT_VAR_PROPERTY(bool, fileIsFile, FileIsFile, false)
    Q_PROPERTY(bool isFile READ getFileIsFile CONSTANT FINAL)

    Q_CONSTANT_VAR_PROPERTY(bool, fileIsLink, FileIsLink, false)
    Q_PROPERTY(bool isLink READ getFileIsLink CONSTANT FINAL)

    Q_CONSTANT_VAR_PROPERTY(bool, fileIsStandardPath, FileIsStandardPath, false)
    Q_PROPERTY(bool isStandardPath READ getFileIsStandardPath CONSTANT FINAL)

    Q_CONSTANT_REF_PROPERTY(QString, name, Name, "")
    Q_CONSTANT_REF_PROPERTY(QString, text, Text, "")
    Q_CONSTANT_REF_PROPERTY(QString, info, Info, "")
    Q_CONSTANT_REF_PROPERTY(QString, group, Group, "")

public:
    FolderTreeObject(const QString& path, const QFileInfo& info, bool fileIsStandardPath=false, QObject* parent = nullptr);
    FolderTreeObject(const QString& path, const QStorageInfo& info, QObject* parent = nullptr);

private:
    void updateName();
    void updateText();
    void updateInfo();
    void updateGroup();
};

class FolderTreeModel: public QObjectTreeModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_REF_PROPERTY(QString, path, Path, "")
    Q_WRITABLE_REF_PROPERTY(QStringList, additionalPaths, AdditionalPaths, {})

    Q_WRITABLE_REF_PROPERTY(QStringList, nameFilters, NameFilters, {})
    Q_WRITABLE_VAR_PROPERTY(bool, caseSensitive, CaseSensitive, false)

    Q_WRITABLE_VAR_PROPERTY(bool, showRootDrives, ShowRootDrives, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showSnapPackageDrives, ShowSnapPackageDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showUnmountedAutofsDrives, ShowUnmountedAutofsDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showTmpfsDrives, ShowTmpfsDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showOverlayDrives, ShowOverlayDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showBootDrives, ShowBootDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showConfigDrives, ShowConfigDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showReadOnlyDrives, ShowReadOnlyDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showQrcDrives, ShowQrcDrives, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showStandardPaths, ShowStandardPaths, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showApplicationDirPath, ShowApplicationDirPath, true)

    Q_WRITABLE_VAR_PROPERTY(bool, showDirs, ShowDirs, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showEmptyDirs, ShowEmptyDirs, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showFiles, ShowFiles, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showDirsFirst, ShowDirsFirst, true)
    Q_WRITABLE_VAR_PROPERTY(bool, showDot, ShowDot, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showDotDot, ShowDotDot, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showDotAndDotDot, ShowDotAndDotDot, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showHidden, ShowHidden, false)
    Q_WRITABLE_VAR_PROPERTY(bool, showOnlyReadable, ShowOnlyReadable, false)

    Q_WRITABLE_VAR_PROPERTY(bool, sortCaseSensitive, SortCaseSensitive, false)
    Q_WRITABLE_VAR_PROPERTY(bool, sortReversed, SortReversed, false)
    Q_WRITABLE_VAR_PROPERTY(FolderTreeModelSortFields::Enum, sortField, SortField, FolderTreeModelSortFields::Unsorted)

    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)

public:
    explicit FolderTreeModel(QObject* parent = nullptr);

    void classBegin() override final;
    void componentComplete() override final;

    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    enum DriveFilter {
        SnapPackage = 0x001,
        UnmountedAutofs = 0x002,
        Tmpfs = 0x004,
        Overlay = 0x008,
        Boot = 0x010,
        Config = 0x020,
        Root = 0x040,
        ReadOnly = 0x080,
        NoFilter = -1
    };
    Q_DECLARE_FLAGS(DriveFilters, DriveFilter)

    QDir::Filters filters() const;
    QDir::SortFlags sortFlags() const;
    DriveFilters driveFilters() const;

    static QList<QStorageInfo> mountedVolumes(bool mount=false);

public slots:
    void queueSelect();
    void select();

    void setStorageList(const QList<QStorageInfo>& storageList);

protected slots:
    void markDirty(const QString &path);

protected:
    static QList<QTreeObject*> fetchPath(const QString &path, QDir::Filters filters, QDir::SortFlags sortFlags, const QStringList& nameFilters);
    static QList<QTreeObject*> fetchDrives(const QList<QStorageInfo>& storageList, const DriveFilters filters);
    static QList<QTreeObject*> fetchQrcDrives();
    static QList<QTreeObject*> fetchStandardPaths();
    static QList<QTreeObject*> fetchApplicationDirPath();

private:
    QFileSystemWatcher* m_watcher=nullptr;
    QTimer m_refreshCaller;

    bool m_selectQueued = false;
};

#endif // FOLDERTREEMODEL_H
