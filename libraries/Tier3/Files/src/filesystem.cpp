#include "filesystem.h"
#include "files_log.h"

#include <QStandardPaths>

#include <Axion>

Filesystem::Filesystem(QObject *parent) :
    QObject(parent),
    m_homePath(QDir::homePath()),
    m_rootPath(QDir::rootPath()),
    m_tempPath(QDir::tempPath()),
    m_appDirPath(QCoreApplication::applicationDirPath()),
    m_appCachePath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)),
    m_appConfigPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)),
    m_desktopPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)),
    m_documentsPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)),
    m_imagesPath(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)),
    m_musicPath(QStandardPaths::writableLocation(QStandardPaths::MusicLocation)),
    m_videosPath(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)),
    m_downloadsPath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)),
    m_workingDirectoryPath(QDir::currentPath()),
    m_drives(new FilesystemDriveModel(this))
{

}

bool Filesystem::isDir(const QString& path)
{
    return QFileInfo(path).isDir();
}

bool Filesystem::isFile(const QString& path)
{
    return QFileInfo(path).isFile();
}

bool Filesystem::isLink(const QString& path)
{
    return QFileInfo(path).isSymLink();
}

bool Filesystem::hasParent(const QString& path)
{
    if(QDir::rootPath() == "/") {
        return(path.length() > 1);
    }
    else {
        return(path.length() > 3);
    }
}

bool Filesystem::exists(const QString& path)
{
    return QFile::exists(path);
}

bool Filesystem::copy(const QString& sourcePath, const QString& destPath, bool force)
{
    bool result = QUtils::Filesystem::copy(sourcePath, destPath, force);

    if(!result) {
        FILESLOG_WARNING()<<"Something went wrong";
    }

    return result;
}

bool Filesystem::move(const QString& sourcePath, const QString& destPath, bool force)
{
    bool result = QUtils::Filesystem::move(sourcePath, destPath, force);

    if(!result) {
        FILESLOG_WARNING()<<"Something went wrong";
    }

    return result;
}

bool Filesystem::link(const QString& sourcePath, const QString& destPath)
{
    return QUtils::Filesystem::link(sourcePath, destPath);
}

bool Filesystem::remove(const QString& path)
{
    return QUtils::Filesystem::remove(path);
}

bool Filesystem::clear(const QString& path)
{
    return QUtils::Filesystem::clear(path);
}

bool Filesystem::mkpath(const QString& path)
{
    return QUtils::Filesystem::mkpath(path);
}

bool Filesystem::mkdir(const QString& path, const QString& name)
{
    return QUtils::Filesystem::mkdir(path, name);
}

bool Filesystem::touch(const QString& path, const QString& name)
{
    return QUtils::Filesystem::touch(path, name);
}

void Filesystem::createFile(const QString& sourcePath)
{
    QVariantMap settings;
    settings["title"] = tr("Nouveau fichier");
    settings["buttonAccept"] = tr("Créer");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showInput(settings);
    dialog->onInputAccepted([sourcePath](const QString& input) mutable {
        bool result = touch(sourcePath, input);
        if(result)
            SnackbarManager::Get()->showSuccess(tr("Fichier %1 créé").arg(input));
        else
            SnackbarManager::Get()->showError(tr("Impossible de créer le fichier %1").arg(input));
    });
}

void Filesystem::createDir(const QString& sourcePath)
{
    QVariantMap settings;
    settings["title"] = tr("Nouveau dossier");
    settings["buttonAccept"] = tr("Créer");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showInput(settings);
    dialog->onInputAccepted([sourcePath](const QString& input) mutable {
        bool result = mkdir(sourcePath, input);
        if(result)
            SnackbarManager::Get()->showSuccess(tr("Dossier %1 créé").arg(input));
        else
            SnackbarManager::Get()->showError(tr("Impossible de créer le dossier %1").arg(input));
    });
}

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif
#include <sys/stat.h>
#ifdef __NetBSD__
#include <sys/statvfs.h>
#endif

int Filesystem::size(const QString& path)
{
    return QFileInfo(path).size();
}

QString Filesystem::sizeInfo(const QString& path)
{
#ifdef __NetBSD__
    struct statvfs info;
    statvfs(path.toLocal8Bit(), &info);
#else
    struct statfs info;
    statfs(path.toLocal8Bit(), &info);
#endif
    if(info.f_blocks == 0) return "";

    qint64 used = (qint64) (info.f_blocks - info.f_bavail)*info.f_bsize;
    qint64 total = (qint64) info.f_blocks*info.f_bsize;
    int percent = (info.f_blocks - info.f_bavail)*100/info.f_blocks;
    return QString("%1 / %2 (%3%)").arg(bytes(used), bytes(total)).arg(percent);
}

QString Filesystem::parentDir(const QString& path)
{
    return QFileInfo(path).dir().absolutePath();
}

QString Filesystem::baseName(const QString& path)
{
    return QFileInfo(path).fileName();
}

QString Filesystem::readTextFile(const QString& path)
{
    QString ret;
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        ret = QString::fromUtf8(file.readAll());
        file.close();
    }
    return ret;
}

bool Filesystem::writeTextFile(const QString& path, const QString& text)
{
    bool ret = false;
    QFile file(path);
    if(file.open(QFile::WriteOnly))
    {
        file.write(text.toUtf8());
        file.flush();
        file.close();
        ret = true;
    }
    return ret;
}

QString Filesystem::pathFromUrl(const QUrl& url)
{
    return url.toLocalFile();
}

QUrl Filesystem::urlFromPath(const QString& path)
{
    return QUrl::fromLocalFile(path);
}

QString Filesystem::nearestExistingAncestorOfPath(const QString& path)
{
    if(QFileInfo::exists(path))
        return path;

    QDir dir(path);
    if(!dir.makeAbsolute())
        return QString();

    do
    {
        dir.setPath(QDir::cleanPath(dir.filePath(QStringLiteral(".."))));
    }
    while(!dir.exists() && !dir.isRoot());

    return dir.exists() ? dir.path() : QString();
}
