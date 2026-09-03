#include "ClockMedia.h"

ClockMedia::ClockMedia(QObject *parent) :
    AbstractManager(parent),
    m_volumeAnimation(new QPropertyAnimation(this))
{
    m_volumeAnimation->setPropertyName("volume");
    m_volumeAnimation->setEasingCurve(QEasingCurve::Linear);

    connect(this, &ClockMedia::mediaPlayerAboutToChange, this, &ClockMedia::onMediaPlayerAboutToChange);
    connect(this, &ClockMedia::mediaPlayerChanged, this, &ClockMedia::onMediaPlayerChanged);
}

bool ClockMedia::init()
{
    return true;
}

void ClockMedia::startMedia(int volume, int fadeInDuration, ClockAlarmMediaStartModes::Enum startMode)
{
    if(!m_mediaPlayer)
    {
        qWarning() << "[ClockMedia] Cannot start media: no media player";
        return;
    }

    qTrace() << "[ClockMedia] Starting media" << m_mediaPlayer->source() << "volume" << volume
            << "fade-in" << fadeInDuration << "seconds" << "start mode" << startMode;

    MediaItemModel* playlist = MediaItemModelAttached::wrap(m_mediaPlayer)->getPlaylist();
    if(!playlist)
    {
        qWarning() << "[ClockMedia] Cannot start media: no playlist";
        return;
    }

    if(startMode == ClockAlarmMediaStartModes::Next)
        playlist->autoNext();

    if(playlist->getCurrentItem().isEmpty())
    {
        qWarning() << "[ClockMedia] Cannot start media: playlist has no current item";
        return;
    }

    QAudioOutput* audioOutput = m_mediaPlayer->audioOutput();
    if(!audioOutput)
    {
        qWarning() << "[ClockMedia] Cannot start media: no audio output";
        return;
    }

    const qreal targetVolume = qBound(0, volume, 100) / 100.0;
    m_volumeAnimation->stop();
    if(fadeInDuration > 0)
    {
        audioOutput->setVolume(0.0);
        m_volumeAnimation->setDuration(fadeInDuration * 1000);
        m_volumeAnimation->setStartValue(0.0);
        m_volumeAnimation->setEndValue(targetVolume);
        m_volumeAnimation->start();
    }
    else
        audioOutput->setVolume(targetVolume);

    QMetaObject::invokeMethod(m_mediaPlayer, &QMediaPlayer::play, Qt::QueuedConnection);
}

void ClockMedia::stopMedia()
{
    if(!m_mediaPlayer)
        return;

    qTrace() << "[ClockMedia] Stopping media" << m_mediaPlayer->source();
    m_mediaPlayer->stop();
    m_volumeAnimation->stop();
}

void ClockMedia::stopFadeIn(int maximumVolume)
{
    if(m_volumeAnimation->state() != QAbstractAnimation::Running)
        return;

    m_volumeAnimation->stop();

    QMediaPlayer* mediaPlayer = m_mediaPlayer;
    QAudioOutput* audioOutput = mediaPlayer ? mediaPlayer->audioOutput() : nullptr;
    if(!audioOutput)
    {
        qWarning() << "[ClockMedia] Cannot stop media fade-in: no audio output";
        return;
    }

    const qreal maximumOutputVolume = qBound(0, maximumVolume, 100) / 100.0;
    if(audioOutput->volume() > maximumOutputVolume)
        audioOutput->setVolume(maximumOutputVolume);

    qTrace() << "[ClockMedia] Media fade-in stopped at" << audioOutput->volume();
}

void ClockMedia::onMediaPlayerAboutToChange(QMediaPlayer* oldMediaPlayer, QMediaPlayer* newMediaPlayer)
{
    if(oldMediaPlayer)
    {
        disconnect(oldMediaPlayer, nullptr, this, nullptr);
        disconnect(this, nullptr, oldMediaPlayer, nullptr);
    }

    m_volumeAnimation->setTargetObject(nullptr);
}

void ClockMedia::onMediaPlayerChanged(QMediaPlayer* mediaPlayer)
{
    if(!mediaPlayer)
        return;

    qTrace() << "[ClockMedia] Media player attached" << mediaPlayer << "source" << mediaPlayer->source();
    connect(mediaPlayer, &QMediaPlayer::errorOccurred, this, &ClockMedia::handleMediaPlayerError);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [mediaPlayer](QMediaPlayer::MediaStatus status) {
        qTrace() << "[ClockMedia] Media status" << status << mediaPlayer->source();
    });
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [mediaPlayer](QMediaPlayer::PlaybackState state) {
        qTrace() << "[ClockMedia] Playback state" << state << mediaPlayer->source();
    });

    m_volumeAnimation->setTargetObject(mediaPlayer->audioOutput());
}

void ClockMedia::handleMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    QMediaPlayer* mediaPlayer = qobject_cast<QMediaPlayer*>(sender());
    qWarning() << "[ClockMedia] Media player error" << error << errorString
               << "source" << (mediaPlayer ? mediaPlayer->source() : QUrl());
}
