#include "paths.h"
#include "axion_log.h"

#include <QUtils>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStorageInfo>

namespace {

QString writableBaseLocation()
{
    static QString baseLocation;
    static bool initialized = false;

    if (initialized)
        return baseLocation;

    const QString applicationDirPath = QCoreApplication::applicationDirPath();
#if defined(Q_OS_BOOT2QT)
    baseLocation = applicationDirPath;
    const QStorageInfo storageInfo(applicationDirPath);
    if (!baseLocation.endsWith(QCoreApplication::applicationName()) && storageInfo.isRoot()) {
        QDir homeDir(QDir::homePath());
        baseLocation = homeDir.filePath(QCoreApplication::organizationName());
        baseLocation = QDir(baseLocation).filePath(QCoreApplication::applicationName());
    }
#elif defined(QT_CREATOR_RUN)
    baseLocation = applicationDirPath;
#else
    baseLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (baseLocation.isEmpty())
        baseLocation = applicationDirPath;
#endif

    baseLocation = QDir::cleanPath(baseLocation);
    initialized = true;

    return baseLocation;
}

QDir writableLocation(const QString& folder, bool mkpath)
{
    const QString cleanFolder = QDir::cleanPath(folder);
    const QString path = cleanFolder.isEmpty() || cleanFolder == QStringLiteral(".") ?
                             writableBaseLocation() :
                             QDir(writableBaseLocation()).filePath(cleanFolder);

    QDir d(path);
    if (mkpath && !d.mkpath(QStringLiteral("."))) {
        AXIONLOG_WARNING() << "Failed to create directory:" << d.absolutePath();
        return {};
    }

    return d;
}

bool ensureFileParentPath(const QFileInfo& fileInfo)
{
    QDir parentDir = fileInfo.dir();
    if (parentDir.exists())
        return true;

    if (!parentDir.mkpath(QStringLiteral("."))) {
        AXIONLOG_WARNING() << "Failed to create directory:" << parentDir.absolutePath();
        return false;
    }

    return true;
}

bool isPathInsideDirectory(const QString& path, const QString& directory)
{
    const QString cleanPath = QDir::cleanPath(path);
    QString cleanDirectory = QDir::cleanPath(directory);

    if (cleanPath == cleanDirectory)
        return true;

    if (!cleanDirectory.endsWith(QLatin1Char('/')))
        cleanDirectory.append(QLatin1Char('/'));

#ifdef Q_OS_WIN
    return cleanPath.startsWith(cleanDirectory, Qt::CaseInsensitive);
#else
    return cleanPath.startsWith(cleanDirectory);
#endif
}

bool isValidStorageFolder(const QString& folder)
{
    if (folder.isEmpty())
        return true;

    const QString cleanFolder = QDir::cleanPath(folder);
    if (cleanFolder == QStringLiteral("."))
        return true;

    if (QFileInfo(cleanFolder).isAbsolute()) {
        AXIONLOG_WARNING() << "Refusing absolute folder in application storage:" << folder;
        return false;
    }

    const QString path = QDir(writableBaseLocation()).filePath(cleanFolder);
    if (!isPathInsideDirectory(path, writableBaseLocation())) {
        AXIONLOG_WARNING() << "Refusing folder outside application storage:" << folder;
        return false;
    }

    return true;
}

}

Paths::Paths(QObject *parent) :
    QObject(parent)
{

}

bool Paths::m_ready = false;
void Paths::init()
{
    if(m_ready)
        return;
    m_ready = true;
#ifdef Q_OS_WASM
#else
    QSettingsMapper::setDefaultPath(setting());
    QSettingsMapper::setDefaultName("settings.conf");
#endif
}

QString Paths::specificLocation(const QString& folder, const QString& file, bool mkpath)
{
    if (!isValidStorageFolder(folder))
        return {};

    QDir d = writableLocation(folder, mkpath && file.isEmpty());
    if (!d.exists() && mkpath && file.isEmpty())
        return {};

    if (file.isEmpty())
        return d.absolutePath() + QLatin1Char('/');

    if (QFileInfo(file).isAbsolute()) {
        AXIONLOG_WARNING() << "Refusing absolute path in application storage:" << file;
        return {};
    }

    const QString filePath = QDir::cleanPath(d.absoluteFilePath(file));
    if (!isPathInsideDirectory(filePath, d.absolutePath())) {
        AXIONLOG_WARNING() << "Refusing path outside application storage:" << file;
        return {};
    }

    const QFileInfo fileInfo(filePath);
    if (mkpath && !ensureFileParentPath(fileInfo))
        return {};

    return filePath;
}

QString Paths::applicationFilePath()
{
    return QCoreApplication::applicationFilePath();
}

QString Paths::applicationFileName()
{
    return QFileInfo(QCoreApplication::applicationFilePath()).fileName();
}

QString Paths::applicationName()
{
    return QCoreApplication::applicationName();
}

QString Paths::local(const QString& file, bool mkpath)
{
    return specificLocation(QString(), file, mkpath);
}

QString Paths::log(const QString& file, bool mkpath)
{
    return specificLocation(QStringLiteral("logs"), file, mkpath);
}

QString Paths::capture(const QString& file, bool mkpath)
{
    return specificLocation(QStringLiteral("captures"), file, mkpath);
}

QString Paths::setting(const QString& file, bool mkpath)
{
    return specificLocation(QStringLiteral("settings"), file, mkpath);
}

QString Paths::database(const QString& file, bool mkpath)
{
    return specificLocation(QStringLiteral("databases"), file, mkpath);
}

QString Paths::cache(const QString& file, bool mkpath)
{
    return specificLocation(QStringLiteral("cache"), file, mkpath);
}
