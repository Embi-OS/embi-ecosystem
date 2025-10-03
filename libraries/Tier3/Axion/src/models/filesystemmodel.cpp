#include "filesystemmodel.h"

#include <QQmlFile>

FileSystemModel::FileSystemModel(QObject* parent):
    QFileSystemModel(parent)
{
    m_roleNames[FileNameRole] = "fileName";
    m_roleNames[FilePathRole] = "filePath";
    m_roleNames[FileBaseNameRole] = "fileBaseName";
    m_roleNames[FileSuffixRole] = "fileSuffix";
    m_roleNames[FileSizeRole] = "fileSize";
    m_roleNames[FileLastModifiedRole] = "fileModified";
    m_roleNames[FileLastReadRole] = "fileAccessed";
    m_roleNames[FileIsDirRole] = "fileIsDir";
    m_roleNames[FileUrlRole] = "fileUrl";

    setResolveSymlinks(true);
    setReadOnly(true);

    connect(this, &FileSystemModel::pathChanged, this, &FileSystemModel::queueInvalidate);
    connect(this, &FileSystemModel::rootUrlChanged, this, &FileSystemModel::queueInvalidate);
    connect(this, &FileSystemModel::nameFiltersChanged, this, &FileSystemModel::queueInvalidate);

    connect(this, &FileSystemModel::showFilesChanged, this, &FileSystemModel::queueInvalidateFilter);
    connect(this, &FileSystemModel::showDirsChanged, this, &FileSystemModel::queueInvalidateFilter);
    connect(this, &FileSystemModel::showDotAndDotDotChanged, this, &FileSystemModel::queueInvalidateFilter);
    connect(this, &FileSystemModel::showHiddenChanged, this, &FileSystemModel::queueInvalidateFilter);
    connect(this, &FileSystemModel::showOnlyReadableChanged, this, &FileSystemModel::queueInvalidateFilter);
    connect(this, &FileSystemModel::caseSensitiveChanged, this, &FileSystemModel::queueInvalidateFilter);
}

void FileSystemModel::classBegin()
{

}

void FileSystemModel::componentComplete()
{
    invalidate();
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    const QFileInfo info(filePath(index));

    switch (role) {
    case FileNameRole:
        return info.fileName();
    case FilePathRole:
        return info.filePath();
    case FileBaseNameRole:
        return info.baseName();
    case FileSuffixRole:
        return info.suffix();
    case FileSizeRole:
        return info.size();
    case FileLastModifiedRole:
        return info.lastModified();
    case FileLastReadRole:
        return info.lastRead();
    case FileIsDirRole:
        return info.isDir();
    case FileUrlRole:
        return QUrl::fromLocalFile(info.filePath());
    default:
        break;
    }
    return QVariant();
}

QHash<int,QByteArray> FileSystemModel::roleNames() const
{
    return m_roleNames;
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : 1;
}

QModelIndex FileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    return QFileSystemModel::index(row, column, parent);
}
QModelIndex FileSystemModel::parent(const QModelIndex &child) const
{
    return QFileSystemModel::parent(child);
}
QModelIndex FileSystemModel::sibling(int row, int column, const QModelIndex &idx) const
{
    return QFileSystemModel::sibling(row, column, idx);
}
bool FileSystemModel::hasChildren(const QModelIndex &parent) const
{
    return QFileSystemModel::hasChildren(parent);
}
bool FileSystemModel::canFetchMore(const QModelIndex &parent) const
{
    return QFileSystemModel::canFetchMore(parent);
}
void FileSystemModel::fetchMore(const QModelIndex &parent)
{
    return QFileSystemModel::fetchMore(parent);
}
int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    return QFileSystemModel::rowCount(parent);
}
Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    return QFileSystemModel::flags(index);
}

void FileSystemModel::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &FileSystemModel::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

QString resolvePath(const QUrl &path)
{
    QString localPath = QQmlFile::urlToLocalFileOrQrc(path);
    QUrl localUrl = QUrl(localPath);
    QString fullPath = localUrl.path();
    if (localUrl.scheme().size())
        fullPath = localUrl.scheme() + QLatin1Char(':') + fullPath;
    return QDir::cleanPath(fullPath);
}

void FileSystemModel::invalidate()
{
    m_completed = true;

    QFileSystemModel::setNameFilters(m_nameFilters);
    invalidateFilter();

    QModelIndex rootIndex;
    if(!m_path.isEmpty())
        rootIndex = setRootPath(m_path);
    else if(!m_rootUrl.isEmpty())
        rootIndex = setRootPath(resolvePath(m_rootUrl));
    else
        rootIndex = setRootPath(QDir::rootPath());

    setRootIndex(rootIndex);
}

void FileSystemModel::queueInvalidateFilter()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_filterInvalidateQueued) {
            m_filterInvalidateQueued = true;
            QMetaObject::invokeMethod(this, &FileSystemModel::invalidateFilter, Qt::QueuedConnection);
        }
    } else {
        invalidateFilter();
    }
}

void FileSystemModel::invalidateFilter()
{
    m_filterInvalidateQueued = false;

    QDir::Filters filter;
    if (m_caseSensitive)
        filter = QDir::CaseSensitive;
    if (m_showFiles)
        filter = filter | QDir::Files;
    if (m_showDirs)
        filter = filter | QDir::AllDirs | QDir::Drives;
    if (!m_showDotAndDotDot)
        filter = filter | QDir::NoDot | QDir::NoDotDot;
    if (m_showHidden)
        filter = filter | QDir::Hidden;
    if (m_showOnlyReadable)
        filter = filter | QDir::Readable;

    setFilter(filter);
}
