#ifndef MEDIAITEMMODEL_H
#define MEDIAITEMMODEL_H

#include <QDefs>
#include <QModels>
#include <QFutureWatcher>
#include <QMediaPlayer>

Q_ENUM_CLASS(MediaPlaybackModes, MediaPlaybackMode,
             Sequential,
             Loop,
             ItemLoop)

class MediaItemModelAttached;
class MediaItemModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_ATTACHED(MediaItemModelAttached)

    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, additionalPaths, AdditionalPaths, {})
    Q_WRITABLE_VAR_PROPERTY(MediaPlaybackModes::Enum, playbackMode, PlaybackMode, MediaPlaybackModes::Sequential)
    Q_WRITABLE_VAR_PROPERTY(bool, shuffled, Shuffled, false)

    Q_WRITABLE_VAR_PROPERTY(int, currentIndex, CurrentIndex, -1)
    Q_PROPERTY(QString currentItem READ getCurrentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(QVariantMap currentMedia READ getCurrentMedia NOTIFY currentIndexChanged)

public:
    explicit MediaItemModel(QObject* parent=nullptr);

    static MediaItemModelAttached* qmlAttachedProperties(QObject* object);

public:
    static QVariantList fetchPath(const QString& path, const QString& currentItem=QString());
    static QVariantList fetchPaths(const QStringList& paths, const QString& currentItem=QString());

    QString getCurrentItem() const;
    QVariantMap getCurrentMedia() const;

public slots:
    void changePlaybackMode();

    void autoNext();
    void next();
    void previous();

    void addPath(const QString& path);

protected:
    bool doSelect() override final;

private slots:
    void onRowsRemoved(const QModelIndex &parent, int first, int last);

private:
    QPointer<QFutureWatcher<QVariantList>> m_selectWatcher;
};

class MediaItemModelAttached : public QObject
{
    Q_OBJECT

    Q_WRITABLE_PTR_PROPERTY(MediaItemModel, playlist, Playlist, nullptr)
    Q_CONSTANT_PTR_PROPERTY(QMediaPlayer, player, player, nullptr)

private:
    friend MediaItemModel;
    explicit MediaItemModelAttached(QMediaPlayer* player);

public:
    static MediaItemModelAttached* wrap(const QObject* object);

private slots:
    void onPlaylistAboutToChange(MediaItemModel* oldMediaPlaylist, MediaItemModel* newMediaPlaylist);
    void onPlaylistChanged(MediaItemModel* mediaPlaylist);

    void onPaylistCurrentItemChanged();
    void onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status);
};

#endif // MEDIAITEMMODEL_H
