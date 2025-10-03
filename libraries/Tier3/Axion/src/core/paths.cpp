#include "paths.h"
#include "axion_log.h"

#include <QUtils>

static QDir writableLocation(const QString& folder)
{
#ifdef Q_OS_BOOT2QT
    const QString baseLocation = QDir::homePath() + "/" +
                                 QCoreApplication::organizationName() + "/" +
                                 QCoreApplication::applicationName();
#else
    const QString baseLocation = QCoreApplication::applicationDirPath();
#endif

    QDir d(baseLocation+"/"+folder);
    if (!d.mkpath(d.absolutePath()))
    {
        AXIONLOG_WARNING()<<"Failed to create directory:"<<d.absolutePath();
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

QString Paths::applicationName()
{
    return QFileInfo(QCoreApplication::applicationFilePath()).fileName();
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
