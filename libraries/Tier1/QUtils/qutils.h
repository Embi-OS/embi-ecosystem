#ifndef QUTILS_H
#define QUTILS_H

#include <QString>
#include <QDebug>
#include <QProcess>
#include <QTimer>

#include <QEventLoop>
#include <QFileInfo>
#include <QDir>

namespace QUtils
{

void printCoreAffinity(int pid=0);
void resetCoreAffinity(int pid=0);
void setCoreAffinity(int coreNo, int pid=0);
void setAppMainCoreAffinity(int coreNo);

bool await(int msecs, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);

#if QT_CONFIG(process)
namespace Process
{

    bool exec(const QString &program, const QStringList &arguments=QStringList(), std::function<void(QProcess*)> callback=nullptr);

}
#endif

namespace Filesystem
{

    bool copy(const QString& sourcePath, const QString& destPath, bool force=false);
    bool move(const QString& sourcePath, QString destPath, bool force=false);
    bool link(const QString& sourcePath, const QString& destPath);
    bool remove(const QString& path);
    bool clear(const QString& path);
    bool mkpath(const QString& path);
    bool mkdir(const QString& path, const QString& name);
    bool touch(const QString& path, const QString& name);

}

}

#endif // QUTILS_H
