#include "mediaitemmodel.h"
#include "media_log.h"

#include <QtConcurrentRun>
#include <QDirIterator>

#include <algorithm>
#include <QSet>

MediaItemModel::MediaItemModel(QObject* parent):
    QVariantListModel(parent)
{
    m_submitPolicy = QVariantListModelPolicies::Disabled;
    m_syncable = false;
    m_primaryField = "path";

    QVariantMap placeholder;
    placeholder.insert("uuid", "");
    placeholder.insert("path", "");
    placeholder.insert("fileUrl", "");
    placeholder.insert("fileName", "");
    placeholder.insert("filePath", "");
    placeholder.insert("fileBaseName", "");
    placeholder.insert("fileCompleteBaseName", "");
    placeholder.insert("fileSuffix", "");
    placeholder.insert("fileCompleteSuffix", "");
    placeholder.insert("fileSize", 0);
    setPlaceholder(placeholder);

    connect(this, &MediaItemModel::pathChanged, this, &MediaItemModel::queueSelect);
    connect(this, &MediaItemModel::additionalPathsChanged, this, &MediaItemModel::queueSelect);
    connect(this, &MediaItemModel::shuffledChanged, this, &MediaItemModel::queueSelect);

    connect(this, &MediaItemModel::rowsRemoved, this, &MediaItemModel::onRowsRemoved);
}

bool MediaItemModel::doSelect()
{
    if(!m_selectWatcher.isNull()) {
        m_selectWatcher->cancel();
        m_selectWatcher->deleteLater();
    }

    bool shuffled = m_shuffled;
    QStringList paths = m_additionalPaths;
    paths.append(m_path);
    std::stable_sort(paths.begin(), paths.end());
    QFuture<QVariantList> future = QtConcurrent::run(&MediaItemModel::fetchPaths, paths, getCurrentMedia().value("path").toString())
    .then(QtFuture::Launch::Async, [shuffled](QVariantList medias){
        if(shuffled)
            QVariantListModel::sort(medias, "uuid");
        else
            QVariantListModel::sort(medias, "path");
        return medias;
    });

    QFutureWatcher<QVariantList>* watcher = new QFutureWatcher<QVariantList>(this);
    m_selectWatcher = watcher;
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
        if(m_selectWatcher != watcher)
            return;
        QVariantList medias = watcher->future().result();

        int currentIndex = 0;
        if(medias.isEmpty())
            currentIndex = -1;
        else if(m_shuffled)
            currentIndex = (std::max)(0, QVariantListModel::indexOf(medias, "uuid", QUuid().toString(QUuid::WithoutBraces), true));
        else
            currentIndex = (std::max)(0, QVariantListModel::indexOf(medias, "path", getCurrentMedia().value("path"), true));

        setStorage(std::move(medias));
        m_currentIndex = currentIndex;
        emit this->currentIndexChanged(m_currentIndex);

        emitSelectDone(true);
    });
    watcher->setFuture(future);

    return true;
}

QString MediaItemModel::getCurrentItem() const
{
    if(m_currentIndex<0 || m_currentIndex>=count())
        return QString();
    return getStorage().at(m_currentIndex).toMap().value("fileUrl").toString();
}

QVariantMap MediaItemModel::getCurrentMedia() const
{
    if(m_currentIndex<0 || m_currentIndex>=count())
        return QVariantMap();
    return getStorage().at(m_currentIndex).toMap();
}

void MediaItemModel::changePlaybackMode()
{
    switch (m_playbackMode) {
    case MediaPlaybackModes::Sequential:
        setPlaybackMode(MediaPlaybackModes::Loop);
        break;
    case MediaPlaybackModes::Loop:
        setPlaybackMode(MediaPlaybackModes::ItemLoop);
        break;
    case MediaPlaybackModes::ItemLoop:
    default:
        setPlaybackMode(MediaPlaybackModes::Sequential);
        break;
    }
}

void MediaItemModel::autoNext()
{
    if(m_playbackMode==MediaPlaybackModes::ItemLoop)
    {
        emit this->currentIndexChanged(m_currentIndex);
        return;
    }

    if(m_playbackMode==MediaPlaybackModes::Sequential && m_currentIndex>=count()-1)
        return;

    next();
}

void MediaItemModel::next()
{
    if(isEmpty()) {
        setCurrentIndex(-1);
        return;
    }

    int index = m_currentIndex + 1;

    switch (m_playbackMode) {
    case MediaPlaybackModes::Loop:
        if(index>=count())
            index=0;
        break;
    case MediaPlaybackModes::ItemLoop:
    case MediaPlaybackModes::Sequential:
    default:
        if(index>=count())
            index--;
        break;
    }

    m_currentIndex = index;
    emit this->currentIndexChanged(m_currentIndex);
}

void MediaItemModel::previous()
{
    if(isEmpty()) {
        setCurrentIndex(-1);
        return;
    }

    int index = m_currentIndex - 1;

    switch (m_playbackMode) {
    case MediaPlaybackModes::Loop:
        if(index<0)
            index=count()-1;
        break;
    case MediaPlaybackModes::ItemLoop:
    case MediaPlaybackModes::Sequential:
    default:
        if(index<0)
            index++;
        break;
    }

    m_currentIndex = index;
    emit this->currentIndexChanged(m_currentIndex);
}

void MediaItemModel::addPath(const QString& path)
{
    if(path.isEmpty())
        return;

    const QFileInfo info(path);
    const QString normalizedPath = info.canonicalFilePath().isEmpty() ? info.absoluteFilePath() : info.canonicalFilePath();
    const QFileInfo primaryInfo(m_path);
    const QString primaryPath = primaryInfo.canonicalFilePath().isEmpty() ? primaryInfo.absoluteFilePath() : primaryInfo.canonicalFilePath();
    if(normalizedPath.isEmpty() || normalizedPath==primaryPath || m_additionalPaths.contains(normalizedPath))
        return;

    m_additionalPaths.append(normalizedPath);
    emit this->additionalPathsChanged(m_additionalPaths);
}

void MediaItemModel::onRowsRemoved(const QModelIndex &parent, int first, int last)
{
    if(parent.isValid())
        return;

    if(isEmpty())
    {
        m_currentIndex = -1;
        emit this->currentIndexChanged(m_currentIndex);
        return;
    }

    if(m_currentIndex>=first && m_currentIndex<=last)
    {
        if(m_currentIndex < size()-1)
        {
            emit this->currentIndexChanged(m_currentIndex);
        }
        else if(!isEmpty())
        {
            m_currentIndex = 0;
            emit this->currentIndexChanged(m_currentIndex);
        }
    }
    else if(m_currentIndex>last)
    {
        m_currentIndex = m_currentIndex - (last-first+1);
        emit this->currentIndexChanged(m_currentIndex);
    }
}

QVariantList MediaItemModel::fetchPath(const QString& path, const QString& currentItem)
{
    QFileInfo info = QFileInfo(path);
    if(!info.exists() || !info.isDir())
    {
        MEDIALOG_WARNING()<<"Can't fetch path:"<<path;
        return QVariantList();
    }

    MEDIALOG_TRACE()<<"Fetch path:"<<path;

    static const QSet<QString> supportedExtensions = {
        "flac",
        "m4a",
        "mp3",
        "ogg",
        "wav",
    };

    QVariantList medias;

    QDirIterator iterator(path, QDir::Files, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QString fileUrl = iterator.next();
        const QFileInfo info = QFileInfo(fileUrl);
        if(!supportedExtensions.contains(info.suffix().toCaseFolded()))
            continue;

        const QString path = info.absoluteFilePath();
        const QUuid uuid = path==currentItem ? QUuid() : QUuid::createUuid();

        QVariantMap media;
        media.insert("uuid", uuid.toString(QUuid::WithoutBraces));
        media.insert("path", info.absoluteFilePath());
        media.insert("fileUrl", QUrl::fromLocalFile(info.absoluteFilePath()).toString());
        media.insert("fileName", info.fileName().isEmpty() ? info.absoluteFilePath() : info.fileName());
        media.insert("filePath", info.absoluteFilePath());
        media.insert("fileBaseName", info.baseName());
        media.insert("fileCompleteBaseName", info.completeBaseName());
        media.insert("fileSuffix", info.suffix());
        media.insert("fileCompleteSuffix", info.completeSuffix());
        media.insert("fileSize", info.size());
        medias.append(media);
    }

    return medias;
}

QVariantList MediaItemModel::fetchPaths(const QStringList& paths, const QString& currentItem)
{
    QVariantList medias;
    QSet<QString> scannedPaths;
    QSet<QString> mediaPaths;

    for(const QString& path: paths)
    {
        if(path.isEmpty())
            continue;

        const QFileInfo info(path);
        const QString normalizedPath = info.canonicalFilePath().isEmpty() ? info.absoluteFilePath() : info.canonicalFilePath();
        if(normalizedPath.isEmpty() || scannedPaths.contains(normalizedPath))
            continue;

        scannedPaths.insert(normalizedPath);
        const QVariantList pathMedias = fetchPath(normalizedPath, currentItem);
        for(const QVariant& media: pathMedias)
        {
            const QString mediaPath = media.toMap().value("path").toString();
            if(mediaPath.isEmpty() || mediaPaths.contains(mediaPath))
                continue;

            mediaPaths.insert(mediaPath);
            medias.append(media);
        }
    }

    return medias;
}

MediaItemModelAttached* MediaItemModel::qmlAttachedProperties(QObject* object)
{
    QMediaPlayer* parent = qobject_cast<QMediaPlayer*>(object);
    if (!parent)
    {
        QMODELSLOG_FATAL("MediaItemModelAttached must be attached to a QMediaPlayer*");
        return nullptr;
    }

    MediaItemModelAttached* helper = new MediaItemModelAttached(parent);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

MediaItemModelAttached::MediaItemModelAttached(QMediaPlayer* player) :
    QObject(player),
    m_playlist(new MediaItemModel(this)),
    m_player(player)
{
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MediaItemModelAttached::onMediaPlayerStatusChanged);

    connect(this, &MediaItemModelAttached::playlistAboutToChange, this, &MediaItemModelAttached::onPlaylistAboutToChange);
    connect(this, &MediaItemModelAttached::playlistChanged, this, &MediaItemModelAttached::onPlaylistChanged);

    onPlaylistChanged(m_playlist);

    m_playlist->setSelectPolicy(QVariantListModelPolicies::Postponed);
    QMetaObject::invokeMethod(m_playlist, &QVariantListModel::select, Qt::QueuedConnection);
}

MediaItemModelAttached* MediaItemModelAttached::wrap(const QObject* object)
{
    return qobject_cast<MediaItemModelAttached*>(qmlAttachedPropertiesObject<MediaItemModel>(object, true));
}

void MediaItemModelAttached::onPlaylistAboutToChange(MediaItemModel* oldMediaPlaylist, MediaItemModel* newMediaPlaylist)
{
    if(oldMediaPlaylist)
    {
        disconnect(oldMediaPlaylist, nullptr, this, nullptr);
        disconnect(this, nullptr, oldMediaPlaylist, nullptr);

        if(oldMediaPlaylist->parent()==this)
            oldMediaPlaylist->deleteLater();
    }
}

void MediaItemModelAttached::onPlaylistChanged(MediaItemModel* mediaPlaylist)
{
    if(!mediaPlaylist)
        return;

    connect(mediaPlaylist, &MediaItemModel::currentIndexChanged, this, &MediaItemModelAttached::onPaylistCurrentItemChanged);

    mediaPlaylist->queueSelect();
}

void MediaItemModelAttached::onPaylistCurrentItemChanged()
{
    if(!m_playlist)
        return;

    if(m_playlist->getCurrentIndex()<0)
    {
        m_player->stop();
        m_player->setSource({});
        return;
    }

    const QUrl source = m_playlist->getCurrentItem();
    const bool wasAtEnd = m_player->mediaStatus()==QMediaPlayer::EndOfMedia;
    if(m_player->source()==source && !wasAtEnd)
        return;

    const bool wasPlaying = m_player->isPlaying() || wasAtEnd;

    m_player->setSource(source);

    if(wasPlaying)
    {
        m_player->play();
    }
    else
    {
        m_player->pause();
    }
}

void MediaItemModelAttached::onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if(!m_playlist)
        return;

    if(status==QMediaPlayer::EndOfMedia)
    {
        m_playlist->autoNext();
    }
}
