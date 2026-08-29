#ifndef CLOCKMEDIA_H
#define CLOCKMEDIA_H

#include <Axion>
#include <Media>
#include <QMediaPlayer>

#include "ClockAlarmAudio.h"
#include <QAudioOutput>
#include <QPropertyAnimation>

struct QMediaPlayerForeign {
    Q_GADGET
    QML_FOREIGN(QMediaPlayer)
    QML_NAMED_ELEMENT(QtMediaPlayer)
};

class ClockMedia : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(ClockMedia)

    Q_WRITABLE_PTR_PROPERTY(QMediaPlayer, mediaPlayer, MediaPlayer, nullptr)

private:
    explicit ClockMedia(QObject *parent = nullptr);

public:
    bool init() final override;

public slots:
    void startMedia(int volume, int fadeInDuration, ClockAlarmMediaStartModes::Enum startMode);
    void stopMedia();

private slots:
    void onMediaPlayerAboutToChange(QMediaPlayer* oldMediaPlayer, QMediaPlayer* newMediaPlayer);
    void onMediaPlayerChanged(QMediaPlayer* mediaPlayer);

    void handleMediaPlayerError(QMediaPlayer::Error error, const QString &errorString);

private:
    QPropertyAnimation* m_volumeAnimation=nullptr;
};

#endif // CLOCKMEDIA_H
