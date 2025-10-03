#include "fstabmodel.h"
#include "files_log.h"

FstabModel::FstabModel(QObject* parent):
    QAbstractListModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &FstabModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &FstabModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &FstabModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &FstabModel::countInvalidate);

    m_roleNames[EntryRole] ="entry";
    m_roleNames[FsSpecRole] ="fsSpec";
    m_roleNames[MountPointRole] ="mountPoint";
    m_roleNames[TypeRole] ="type";
    m_roleNames[OptionsRole] ="options";
    m_roleNames[DumpRole] ="dump";
    m_roleNames[PassRole] ="pass";

    refresh();
}

QVariant FstabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();

    const FstabEntry entry = m_entries.at(index.row());

    switch (role) {
    case EntryRole:
        return QVariant::fromValue<FstabEntry>(entry);
    case FsSpecRole:
        return entry.fsSpec();
    case MountPointRole:
        return entry.mountPoint();
    case TypeRole:
        return entry.type();
    case OptionsRole:
        return entry.options();
    case DumpRole:
        return entry.dumpFreq();
    case PassRole:
        return entry.passNumber();
    }

    return QVariant();
}

int FstabModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_entries.size();
}

QHash<int, QByteArray> FstabModel::roleNames() const
{
    return m_roleNames;
}

const QList<FstabEntry>& FstabModel::entries() const
{
    return m_entries;
}

FstabEntry FstabModel::entry(int index) const
{
    if(index<0 || index>=m_entries.size())
        return FstabEntry();
    return m_entries.at(index);
}

QVariantMap FstabModel::smbParams(const QString& mountPoint) const
{
    int index = QModelHelper::indexOf(this, "mountPoint", mountPoint);
    if(index<0 || index>=m_entries.size())
    {
        FILESLOG_WARNING()<<mountPoint<<"does not exists";
        return QVariantMap();
    }
    const FstabEntry entry = m_entries.at(index);

    return entry.toSmbParams();
}

void FstabModel::refresh()
{
    const QList<FstabEntry> entries = FstabEntry::entries();

    if(m_entries.size()!=entries.size())
    {
        beginResetModel();
        m_entries = entries;
        endResetModel();
    }
    else
    {
        m_entries = entries;
        emit this->dataChanged(index(0), index(m_entries.size()-1));
    }

    emit updated();
}

bool FstabModel::addSmbShare(const QVariantMap& mountParams)
{
    const FstabEntry data = FstabEntry::fromSmbParams(mountParams, true);
    if(!data.isValid())
    {
        FILESLOG_CRITICAL()<<"Invalid FsTabEntry:"<<data;
        emit this->error("Invalid FsTabEntry");
        return false;
    }

    if(QModelHelper::contains(this, "mountPoint", data.mountPoint()))
    {
        FILESLOG_WARNING()<<data<<"already exists";
        emit this->error(QString("%1 already exists").arg(data.mountPoint()));
        return true;
    }

    if(!QDir(data.mountPoint()).exists())
    {
        if(!QDir().mkpath(data.mountPoint()))
        {
            FILESLOG_WARNING()<<"Failed to mkdir"<<data.mountPoint();
            emit this->error(QString("Failed to mkdir %1").arg(data.mountPoint()));
            return false;
        }
    }
    else if(!QDir(data.mountPoint()).isEmpty())
    {
        FILESLOG_WARNING()<<data.mountPoint()<<"is not empty";
        emit this->error(QString("%1 is not empty").arg(data.mountPoint()));
        return false;
    }

    FILESLOG_INFO().noquote()<<data;

    QList<FstabEntry> entries = FstabEntry::entries();
    entries.append(data);
    bool result = FstabEntry::writeMountpoints(entries);

    mount();
    refresh();

    return result;
}

bool FstabModel::updateSmbShare(const QString& mountPoint, const QVariantMap& mountParams)
{
    const FstabEntry data = FstabEntry::fromSmbParams(mountParams, true);
    if(!data.isValid())
    {
        FILESLOG_CRITICAL()<<"Invalid FsTabEntry:"<<data;
        emit this->error("Invalid FsTabEntry");
        return false;
    }

    if(!QModelHelper::contains(this, "mountPoint", mountPoint))
    {
        FILESLOG_WARNING()<<mountPoint<<"does not exists";
        emit this->error(QString("%1 does not exists").arg(mountPoint));
        return true;
    }

    QList<FstabEntry> entries = FstabEntry::entries();
    int index = FstabEntry::indexOf(entries, mountPoint);
    if(index<0)
        return false;
    entries.replace(index, data);

    umount(mountPoint);
    bool result = FstabEntry::writeMountpoints(entries);
    mount();
    refresh();

    return result;
}

bool FstabModel::removeSmbShare(const QString& mountPoint)
{
    if(!QModelHelper::contains(this, "mountPoint", mountPoint))
    {
        FILESLOG_WARNING()<<mountPoint<<"does not exists";
        return true;
    }

    QList<FstabEntry> entries = FstabEntry::entries();
    int index = FstabEntry::indexOf(entries, mountPoint);
    if(index<0)
        return false;
    entries.remove(index);

    umount(mountPoint);
    bool result = FstabEntry::writeMountpoints(entries);
    mount();
    refresh();

    return result;
}

void FstabModel::testSmbShare(const QVariantMap& mountParams)
{
#if QT_CONFIG(process)
    FstabEntry data = FstabEntry::fromSmbParams(mountParams, false);
    if(data.fsSpec().isEmpty())
    {
        FILESLOG_CRITICAL()<<"No hostname given";
        return;
    }

    QString fakeMountPoint = data.mountPoint();
    fakeMountPoint.append("_fake");
    bool tmpDir=false;
    if(!QDir(fakeMountPoint).exists() && QDir().mkpath(fakeMountPoint))
        tmpDir = true;

    QString program="mount";
    QStringList arguments = QStringList()<<data.fsSpec()<<fakeMountPoint;

    setCurrentProcess(program+" "+arguments.join(" "));
    setProcessRunning(true);

    if(!data.type().isEmpty())
        arguments<<"-t"<<data.type();
    if(!data.options().isEmpty())
        arguments<<"-o"<<data.optionsString();

    QUtils::Process::exec(program, arguments, [this, data, fakeMountPoint, tmpDir](QProcess* process){
        bool result = process->exitCode()==0;

        if(!result)
        {
            setProcessRunning(false);
            setCurrentProcess("");
            if(tmpDir)
                QDir().rmdir(fakeMountPoint);
            emit this->error(QString("failed to mount: %1").arg(process->readAllStandardError()));
            emit this->smbShareTested(data.fsSpec(), result);
            return;
        }

        QUtils::Process::exec("umount", QStringList()<<fakeMountPoint, [this, data, fakeMountPoint, tmpDir](QProcess* process){
            setProcessRunning(false);
            setCurrentProcess("");
            bool result = process->exitCode()==0;
            if(tmpDir)
                QDir().rmdir(fakeMountPoint);
            emit this->smbShareTested(data.fsSpec(), result);
        });
    });
#else
    emit this->error(QString("failed to mount: %1").arg("QProcess is not available"));
#endif
}

void FstabModel::mount()
{
#if QT_CONFIG(process)
    QString program="mount";
    QStringList arguments = QStringList()<<"-a";

    setCurrentProcess(program+" "+arguments.join(" "));
    setProcessRunning(true);

    QUtils::Process::exec(program, arguments, [this](QProcess* process){
        setProcessRunning(false);
        setCurrentProcess("");
        bool result = process->exitCode()==0;
        if(!result)
            emit this->error(QString("failed to mount: %1").arg(process->readAllStandardError()));
        emit this->mounted("", result);
    });
#else
    emit this->error(QString("failed to mount: %1").arg("QProcess is not available"));
#endif
}

void FstabModel::umount(const QString& mountPoint)
{
#if QT_CONFIG(process)
    QString program="umount";
    QStringList arguments = QStringList()<<mountPoint;

    setCurrentProcess(program+" "+arguments.join(" "));
    setProcessRunning(true);

    QUtils::Process::exec(program, arguments, [this, mountPoint](QProcess* process){
        setProcessRunning(false);
        setCurrentProcess("");
        bool result = process->exitCode()==0;
        if(!result)
            emit this->error(QString("failed to umount: %1").arg(process->readAllStandardError()));
        emit this->umounted(mountPoint, result);
    });
#else
    emit this->error(QString("failed to umount: %1").arg("QProcess is not available"));
#endif
}

void FstabModel::countInvalidate()
{
    const int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}
