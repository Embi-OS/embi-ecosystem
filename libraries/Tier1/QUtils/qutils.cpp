#include "qutils.h"
#include "qutils_log.h"

#ifdef Q_OS_LINUX
#include <sys/syscall.h>
#include <unistd.h>
#ifdef QT_CONCURRENT_LIB
#include <QtConcurrentRun>
#include <QFutureSynchronizer>
#endif
#endif

bool QUtils::await(int msecs, QEventLoop::ProcessEventsFlags flags)
{
    QEventLoop loop;

    QUTILSLOG_DEBUG()<<"Waiting"<<msecs<<"[ms]";

    QTimer::singleShot(msecs, &loop, [&loop](){
        if(loop.isRunning())
            loop.quit();
    });

    loop.exec(flags);

    return true;
}

#if QT_CONFIG(process)
bool QUtils::Process::exec(const QString &program, const QStringList &arguments, std::function<void(QProcess*)> callback)
{
    QProcess *process = new QProcess();
    process->setProgram(program);
    process->setArguments(arguments);

    QObject::connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     [process, callback](int exitCode, QProcess::ExitStatus exitStatus)
    {
        if(callback)
            callback(process);
        process->deleteLater();
    });

    process->start();

    return true;
}
#endif

bool QUtils::Filesystem::copy(const QString& sourcePath, const QString& destPath, bool force)
{
    if(sourcePath==destPath)
        return true;

    QFileInfo src = QFileInfo(sourcePath);
    QFileInfo dst = QFileInfo(destPath);

    if(!src.exists())
    {
        QUTILSLOG_WARNING()<<"Source:"<<sourcePath<<"does not exist";
        return false;
    }

    if(src.isDir())
    {
        if(!dst.dir().mkpath(destPath))
        {
            QUTILSLOG_WARNING()<<"Cannot create destination"<<destPath;
            return false;
        }
    }

    if(!dst.dir().exists())
    {
        if(!dst.dir().mkpath(dst.dir().absolutePath()))
        {
            QUTILSLOG_WARNING()<<"Cannot create destination path"<<dst.dir().absolutePath();
            return false;
        }
    }

    if(src.isFile())
    {
        if(force && QFile::exists(destPath))
        {
            QFile::remove(destPath);
        }
        return QFile::copy(sourcePath, destPath);
    }

    QDir dir(sourcePath);
    bool result=true;

    const QStringList dirEntries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);
    for(const QString& d: dirEntries)
    {
        QString src_path = sourcePath + QDir::separator() + d;
        QString dst_path = destPath + QDir::separator() + d;
        if(!copy(src_path, dst_path, force))
            result=false;
    }

    return result;
}

bool QUtils::Filesystem::move(const QString& sourcePath, QString destPath, bool force)
{
    if(sourcePath==destPath)
        return true;

    QFileInfo src = QFileInfo(sourcePath);
    QFileInfo dst = QFileInfo(destPath);

    if(!src.exists())
    {
        QUTILSLOG_WARNING()<<"Source:"<<sourcePath<<"does not exists";
        return false;
    }

    if(src.isDir())
    {
        if(!dst.dir().mkpath(destPath))
        {
            QUTILSLOG_WARNING()<<"Cannot create destination"<<destPath;
            return false;
        }
    }

    if(src.isFile() && dst.isDir())
    {
        destPath = QFileInfo(destPath+"/"+src.fileName()).absoluteFilePath();
        dst = QFileInfo(destPath);
    }

    if(!dst.dir().exists())
    {
        if(!dst.dir().mkpath(dst.dir().absolutePath()))
        {
            QUTILSLOG_WARNING()<<"Cannot create destination path"<<dst.dir().absolutePath();
            return false;
        }
    }

    if(src.isFile())
    {
        if(force && QFile::exists(destPath))
        {
            QFile::remove(destPath);
        }
        return QFile::rename(sourcePath, destPath);
    }

    QDir dir(sourcePath);
    bool result=true;

    const QStringList dirEntries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);
    for(const QString& d: dirEntries)
    {
        QString src_path = sourcePath + QDir::separator() + d;
        QString dst_path = destPath + QDir::separator() + d;
        if(!move(src_path, dst_path, force))
            result=false;
    }

    if(!result)
        return false;

    return remove(sourcePath);
}

bool QUtils::Filesystem::link(const QString& sourcePath, const QString& destPath)
{
    return QFile::link(sourcePath, destPath);
}

bool QUtils::Filesystem::remove(const QString& path)
{
    if(QFileInfo(path).isDir())
    {
        return QDir(path).removeRecursively();
    }
    else
    {
        return QFile::remove(path);
    }
}

bool QUtils::Filesystem::clear(const QString& path)
{
    QDir dir(path);
    if(! dir.exists())
        return true;

    const QStringList entryList = dir.entryList();
    for(const QString & filename: entryList)
    {
        dir.remove(filename);
    }

    return true;
}

bool QUtils::Filesystem::mkpath(const QString& path)
{
    return QDir().mkpath(path);
}

bool QUtils::Filesystem::mkdir(const QString& path, const QString& name)
{
    return QDir(path).mkdir(name);
}

bool QUtils::Filesystem::touch(const QString& path, const QString& name)
{
    return QFile(QDir(path).filePath(name)).open(QIODevice::WriteOnly);
}
