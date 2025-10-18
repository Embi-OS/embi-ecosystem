#include "paths.h"
#include "axion_log.h"

#include <QUtils>
#include <QStorageInfo>

static QDir writableLocation(const QString& folder)
{
    static QString baseLocation;
    static bool initialized = false;

    if (!initialized) {
        const QString applicationDirPath = QCoreApplication::applicationDirPath();
        baseLocation = applicationDirPath;
#if defined(Q_OS_BOOT2QT) || !defined(QT_CREATOR_RUN)
        const QStorageInfo storageInfo(applicationDirPath);
        if (!baseLocation.endsWith(QCoreApplication::applicationName()) && storageInfo.isRoot()) {
            baseLocation = QDir::homePath() + "/" +
                           QCoreApplication::organizationName() + "/" +
                           QCoreApplication::applicationName();
        }
#endif
        initialized = true;
    }

    QDir d(baseLocation + "/" + folder);
    if (!d.mkpath(d.absolutePath())) {
        AXIONLOG_WARNING() << "Failed to create directory:" << d.absolutePath();
        return QDir();
    }

    return d;
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

QString Paths::specificLocation(const QString& folder, const QString& file)
{
    QDir d = writableLocation(folder);
    if (file.isEmpty())
        return d.absolutePath() + QDir::separator();
    return d.filePath(file);
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

QString Paths::local(const QString& file)
{
    QString folder = "";
    return specificLocation(folder, file);
}

QString Paths::log(const QString& file)
{
    QString folder = "logs";
    return specificLocation(folder, file);
}

QString Paths::capture(const QString& file)
{
    QString folder = "captures";
    return specificLocation(folder, file);
}

QString Paths::setting(const QString& file)
{
    QString folder = "settings";
    return specificLocation(folder, file);
}

QString Paths::database(const QString& file)
{
    QString folder = "databases";
    return specificLocation(folder, file);
}

QString Paths::cache(const QString& file)
{
    QString folder = "cache";
    return specificLocation(folder, file);
}
