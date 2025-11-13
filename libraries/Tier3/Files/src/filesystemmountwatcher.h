#ifndef FILESYSTEMMOUNTWATCHER_H
#define FILESYSTEMMOUNTWATCHER_H

#include <QObject>
#include <QMultiMap>

class FileSystemMountWatcherPrivate;
class FileSystemMountWatcher : public QObject
{
    Q_OBJECT

public:
    FileSystemMountWatcher(QObject *parent = nullptr);
    ~FileSystemMountWatcher() override;

    static QMultiMap<QString, QString> currentMountPoints();

    void addMountPoint(const QString &directory);
    void removeMountPoint(const QString &directory);

    // Auto-test API, only functional on Linux. Needs to be called before constructor:
    static bool setMountTabFileForTesting(const QString &mtabFile);

Q_SIGNALS:
    void mountAdded(const QString &mountPoint, const QString &device);
    void mountRemoved(const QString &mountPoint, const QString &device);
    void mountChanged(const QString &mountPoint, const QString &device);

private:
    static FileSystemMountWatcherPrivate *d;
};

#endif // FILESYSTEMMOUNTWATCHER_H
