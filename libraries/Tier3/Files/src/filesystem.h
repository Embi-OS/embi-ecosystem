#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QDefs>
#include <QUtils>
#include "qsingleton.h"

#include "filesystemdrivemodel.h"

class Filesystem : public QObject,
                   public QQmlSingleton<Filesystem>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_REF_PROPERTY(QString, homePath, HomePath, "")
    Q_CONSTANT_REF_PROPERTY(QString, rootPath, RootPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, tempPath, TempPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, appDirPath, AppDirPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, appCachePath, AppCachePath, "")
    Q_CONSTANT_REF_PROPERTY(QString, appConfigPath, AppConfigPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, desktopPath, DesktopPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, documentsPath, DocumentsPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, imagesPath, ImagesPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, musicPath, MusicPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, videosPath, VideosPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, downloadsPath, DownloadsPath, "")
    Q_CONSTANT_REF_PROPERTY(QString, workingDirectoryPath, WorkingDirectoryPath, "")
    Q_CONSTANT_PTR_PROPERTY(FilesystemDriveModel, drives, Drives, nullptr)

    Q_WRITABLE_REF_PROPERTY(QString, pathClipboard, PathClipboard, "")

protected:
    friend QQmlSingleton<Filesystem>;
    explicit Filesystem(QObject *parent = nullptr);

public:
    enum Permission {
        ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
        ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
        ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
        ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001,
    };
    Q_ENUM (Permission)

    Q_INVOKABLE static bool isDir(const QString& path);
    Q_INVOKABLE static bool isFile(const QString& path);
    Q_INVOKABLE static bool isLink(const QString& path);
    Q_INVOKABLE static bool hasParent(const QString& path);
    Q_INVOKABLE static bool exists(const QString& path);

    Q_INVOKABLE static int size(const QString& path);
    Q_INVOKABLE static QString sizeInfo(const QString& path);

    Q_INVOKABLE static QString parentDir(const QString& path);
    Q_INVOKABLE static QString baseName(const QString& path);

    Q_INVOKABLE static QString readTextFile(const QString& path);
    Q_INVOKABLE static bool writeTextFile(const QString& path, const QString& text);

    Q_INVOKABLE static QString pathFromUrl(const QUrl& url);
    Q_INVOKABLE static QUrl urlFromPath(const QString& path);

    Q_INVOKABLE static QString nearestExistingAncestorOfPath(const QString& path);

public slots:
    static bool copy(const QString& sourcePath, const QString& destPath, bool force=false);
    static bool move(const QString& sourcePath, const QString& destPath, bool force=false);
    static bool link(const QString& sourcePath, const QString& destPath);
    static bool remove(const QString& path);
    static bool clear(const QString& path);
    static bool mkpath(const QString& path);
    static bool mkdir(const QString& path, const QString& name);
    static bool touch(const QString& path, const QString& name);

    static void createFile(const QString& sourcePath);
    static void createDir(const QString& sourcePath);
};

#endif // FILESYSTEM_H
