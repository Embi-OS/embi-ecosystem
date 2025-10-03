#include "mimeiconhelper.h"

#include <QIcon>
#include <QFile>
#include <QFileInfo>
#include <QStorageInfo>
#include <QStandardPaths>
#include <QUrl>
#include <QMutex>
#include <QDebug>
#include <QMimeDatabase>

class MetaDataCache {
public:
    explicit MetaDataCache (void);

    void registerSpecialFolderIconForPath (const QString & path, const QString & icon);
    void registerSvgIconForMimeType       (const QString & type, const QString & icon);
    void registerSvgIconForFsType         (const QString & type, const QString & icon);
    void registerThemeIconForMimeType     (const QString & type, const QString & icon);

    QString getSpecialFolderIconForPath (const QString & path);
    QString getSvgIconForMimeType       (const QString & type);
    QString getThemeIconForMimeType     (const QString & type);
    QString getSvgIconForFsType         (const QString & type);

    QMimeType getMimeTypeForFile (const QString & path);

private:
    void registerStandardMimetypes();

    QMimeDatabase mimeDb;
    QHash<QString, QString> svgIconForMimetype;
    QHash<QString, QString> themeIconNameForMimeType;
    QHash<QString, QString> specialFoldersIconNames;
    QHash<QString, QString> svgIconForFstype;
};

Q_GLOBAL_STATIC(MetaDataCache, mimeCache)

MimeIconHelper::MimeIconHelper(QObject *parent) :
    QObject(parent)
{

}

QString MimeIconHelper::getSvgIconPathForUrl(const QString& url, bool isOpen) const
{
    QString ret;
    const QString path = QUrl (url).toLocalFile ();
    const QStorageInfo storage (path);

    if(storage.isValid() && storage.isReady() && storage.rootPath()==path) {
        const QString tmp = storage.isRoot() ? "root" :
                            (storage.rootPath().startsWith("/boot") || storage.name().toLower().contains("boot")) ? "boot" :
                            storage.fileSystemType ();
        ret = mimeCache->getSvgIconForFsType (tmp.toLower());
    }

    if (ret.isEmpty ()) {
        const QFileInfo info (path);
        if (info.exists ()) {
            if (info.isDir ()) {
                if(isOpen)
                    ret = "folder-opened";
                else {
                    ret = mimeCache->getSpecialFolderIconForPath (path);
                    if (ret.isEmpty ()) {
                        ret = "folder-closed";
                    }
                }
            }
            else {
                const QMimeType type = mimeCache->getMimeTypeForFile (path);
                const QString tmp = type.name ();
                ret = mimeCache->getSvgIconForMimeType (tmp);
                if (ret.isEmpty ()) {
                    ret = "file";
                }
            }
        }
    }

    if (ret.isEmpty ()) {
        ret = "file-unknown";
    }
    return QStringLiteral ("qrc:///images/filetypes/%1.svg").arg (ret);
}

QColor MimeIconHelper::getIconColorForUrl(const QString& url) const
{
    QColor ret;
    const QString path = QUrl (url).toLocalFile ();
    const QFileInfo info (path);
    if (info.exists ()) {
        if (info.isDir ()) {
            ret = mimeCache->getSpecialFolderIconForPath (path);
            if (!ret.isValid ()) {
                ret = "transparent";
            }
        }
        else {
            const QMimeType type = mimeCache->getMimeTypeForFile (path);
            const QString tmp = type.name ();
            ret = mimeCache->getSvgIconForMimeType (tmp);
            if (!ret.isValid ()) {
                ret = "transparent";
            }
        }
    }
    return ret;
}

QString MimeIconHelper::getThemeIconNameForUrl(const QString& url) const
{
    QString ret;
    const QString path = QUrl (url).toLocalFile ();
    const QFileInfo info (path);
    if (info.exists ()) {
        if (info.isDir ()) {
            ret = mimeCache->getSpecialFolderIconForPath (path);
            if (ret.isEmpty ()) {
                ret = "folder";
            }
        }
        else {
            const QMimeType type = mimeCache->getMimeTypeForFile (path);
            const QString tmp = type.name ();
            ret = mimeCache->getThemeIconForMimeType (tmp);
            if (ret.isEmpty ()) {
                if (QIcon::hasThemeIcon (type.iconName ())) {
                    mimeCache->registerThemeIconForMimeType (tmp, type.iconName ());
                }
                else if (QIcon::hasThemeIcon (type.genericIconName ())) {
                    mimeCache->registerThemeIconForMimeType (tmp, type.genericIconName ());
                }
                else {
                    mimeCache->registerThemeIconForMimeType (tmp, "empty");
                }
            }
            ret = mimeCache->getThemeIconForMimeType (tmp);
        }
    }
    return ret;
}

void MetaDataCache::registerSpecialFolderIconForPath (const QString & path, const QString & icon) {
    specialFoldersIconNames.insert (path, icon);
}

void MetaDataCache::registerSvgIconForMimeType (const QString & type, const QString & icon) {
    svgIconForMimetype.insert (type, icon);
}

void MetaDataCache::registerThemeIconForMimeType (const QString & type, const QString & icon) {
    themeIconNameForMimeType.insert (type, icon);
}

void MetaDataCache::registerSvgIconForFsType (const QString & type, const QString & icon) {
    svgIconForFstype.insert (type, icon);
}

QMimeType MetaDataCache::getMimeTypeForFile (const QString & path) {
    return mimeDb.mimeTypeForFile (path);
}

QString MetaDataCache::getSpecialFolderIconForPath (const QString & path) {
    return specialFoldersIconNames.value (path);
}

QString MetaDataCache::getSvgIconForMimeType (const QString & type) {
    return svgIconForMimetype.value (type);
}

QString MetaDataCache::getThemeIconForMimeType (const QString & type) {
    return themeIconNameForMimeType.value (type);
}

QString MetaDataCache::getSvgIconForFsType (const QString & type) {
    return svgIconForFstype.value (type);
}

MetaDataCache::MetaDataCache (void) {
    svgIconForMimetype.reserve (100);
    specialFoldersIconNames.reserve (20);
    themeIconNameForMimeType.reserve (100);
    svgIconForFstype.reserve (100);

    registerStandardMimetypes();
}

void MetaDataCache::registerStandardMimetypes()
{

    /// register special folders

    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::HomeLocation)) {
        registerSpecialFolderIconForPath (path, "folder-home");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::DesktopLocation)) {
        registerSpecialFolderIconForPath (path, "folder-desktop");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation)) {
        registerSpecialFolderIconForPath (path, "folder-documents");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::PicturesLocation)) {
        registerSpecialFolderIconForPath (path, "folder-images");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::MusicLocation)) {
        registerSpecialFolderIconForPath (path, "folder-music");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::MoviesLocation)) {
        registerSpecialFolderIconForPath (path, "folder-videos");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::DownloadLocation)) {
        registerSpecialFolderIconForPath (path, "folder-downloads");
    }
    for (const QString & path: QStandardPaths::standardLocations (QStandardPaths::HomeLocation)) {
        const QString trashPath = QString("%1/.local/share/Trash").arg(path);
        registerSpecialFolderIconForPath (trashPath, "folder-trash");
    }
    registerSpecialFolderIconForPath (QCoreApplication::applicationDirPath(), "folder-local");
    registerSpecialFolderIconForPath (":/", "drive-qrc");

    /// register SVG icons for MIME-types

    registerSvgIconForMimeType ("image/png", "file-image");
    registerSvgIconForMimeType ("image/jpeg", "file-image");
    registerSvgIconForMimeType ("image/gif", "file-image");

    registerSvgIconForMimeType ("image/svg", "file-drawing");
    registerSvgIconForMimeType ("image/svg+xml", "file-drawing");
    registerSvgIconForMimeType ("application/vnd.oasis.opendocument.graphics", "file-drawing");

    registerSvgIconForMimeType ("audio/mpeg", "file-sound");
    registerSvgIconForMimeType ("audio/x-wav", "file-sound");
    registerSvgIconForMimeType ("audio/midi", "file-sound");

    registerSvgIconForMimeType ("video/mp4", "file-video");

    registerSvgIconForMimeType ("text/x-csrc", "file-code");
    registerSvgIconForMimeType ("text/x-chdr", "file-code");
    registerSvgIconForMimeType ("text/x-c++src", "file-code");
    registerSvgIconForMimeType ("text/x-c++hdr", "file-code");
    registerSvgIconForMimeType ("text/x-qml", "file-code");
    registerSvgIconForMimeType ("text/x-java", "file-code");
    registerSvgIconForMimeType ("text/x-python3", "file-code");
    registerSvgIconForMimeType ("text/css", "file-code");
    registerSvgIconForMimeType ("application/javascript", "file-code");

    registerSvgIconForMimeType ("application/xml", "file-xml");

    registerSvgIconForMimeType ("application/x-shellscript", "file-script");
    registerSvgIconForMimeType ("application/x-perl", "file-script");

    registerSvgIconForMimeType ("application/x-object", "file-binary");
    registerSvgIconForMimeType ("application/octet-stream", "file-binary");

    registerSvgIconForMimeType ("application/x-cd-image", "file-disk-image");

    registerSvgIconForMimeType ("application/zip", "archive");
    registerSvgIconForMimeType ("application/x-xz-compressed-tar", "archive");
    registerSvgIconForMimeType ("application/x-compressed-tar", "archive");
    registerSvgIconForMimeType ("application/x-rar", "archive");
    registerSvgIconForMimeType ("application/x-tar", "archive");
    registerSvgIconForMimeType ("application/x-rpm", "archive");
    registerSvgIconForMimeType ("application/gzip", "archive");
    registerSvgIconForMimeType ("application/vnd.debian.binary-package", "archive");
    registerSvgIconForMimeType ("application/vnd.android.package-archive", "archive");
    registerSvgIconForMimeType ("application/x-7z-compressed", "archive");
    registerSvgIconForMimeType ("application/x-bzip-compressed-tar", "archive");
    registerSvgIconForMimeType ("application/vnd.rar", "archive");

    registerSvgIconForMimeType ("text/x-makefile", "file-text");
    registerSvgIconForMimeType ("text/x-log", "file-text");
    registerSvgIconForMimeType ("text/x-theme", "file-text");
    registerSvgIconForMimeType ("text/csv", "file-text");
    registerSvgIconForMimeType ("text/plain", "file-text");
    registerSvgIconForMimeType ("text/vcard", "file-text");
    registerSvgIconForMimeType ("text/markdown", "file-text");
    registerSvgIconForMimeType ("application/json", "file-text");

    registerSvgIconForMimeType ("application/pdf", "file-pdf");

    registerSvgIconForMimeType ("application/vnd.oasis.opendocument.text", "file-document");
    registerSvgIconForMimeType ("application/vnd.openxmlformats-officedocument.wordprocessingml.document", "file-document");
    registerSvgIconForMimeType ("application/msword", "file-document");

    registerSvgIconForMimeType ("application/vnd.oasis.opendocument.spreadsheet", "file-spreadsheet");
    registerSvgIconForMimeType ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "file-spreadsheet");
    registerSvgIconForMimeType ("application/vnd.ms-excel", "file-spreadsheet");
    registerSvgIconForMimeType ("application/ms-excel", "file-spreadsheet");

    registerSvgIconForMimeType ("application/vnd.oasis.opendocument.presentation", "file-slideshow");
    registerSvgIconForMimeType ("application/vnd.openxmlformats-officedocument.presentationml.presentation", "file-slideshow");
    registerSvgIconForMimeType ("application/vnd.ms-powerpoint", "file-slideshow");

    registerSvgIconForMimeType ("text/html", "file-webpage");

    registerSvgIconForMimeType ("application/sql", "file-database");
    registerSvgIconForMimeType ("application/x-sqlite3", "file-database");
    registerSvgIconForMimeType ("application/vnd.sqlite3", "file-database");

    registerSvgIconForMimeType ("application/x-executable", "file-executable");
    registerSvgIconForMimeType ("application/x-ms-dos-executable", "file-executable");

    /// register SVG icons for FS-types

    registerSvgIconForFsType ("root", "drive-root");
    registerSvgIconForFsType ("boot", "drive-boot");
    registerSvgIconForFsType ("overlay", "drive-virtual");

    registerSvgIconForFsType ("ext", "drive");
    registerSvgIconForFsType ("ext2", "drive");
    registerSvgIconForFsType ("ext3", "drive");
    registerSvgIconForFsType ("ext4", "drive");
    registerSvgIconForFsType ("fuseblk", "drive");

    registerSvgIconForFsType ("exfat", "drive-usb");
    registerSvgIconForFsType ("vfat", "drive-usb");
    registerSvgIconForFsType ("ntfs", "drive-usb");
    registerSvgIconForFsType ("ntfs-3g", "drive-usb");

    registerSvgIconForFsType ("nfs", "drive-network");
    registerSvgIconForFsType ("smb", "drive-network");
    registerSvgIconForFsType ("cifs", "drive-network");

}
