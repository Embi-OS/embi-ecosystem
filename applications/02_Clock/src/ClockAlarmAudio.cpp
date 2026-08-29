#include "ClockAlarmAudio.h"

#include "ClockMedia.h"

#include <helpers/alarmobject.h>

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QPropertyAnimation>

ClockAlarmAudio::ClockAlarmAudio(QObject* parent) :
    AbstractManager(parent),
    m_ringtonePlayer(new QMediaPlayer(this)),
    m_ringtoneAudioOutput(new QAudioOutput(this)),
    m_volumeAnimation(new QPropertyAnimation(this)),
    m_ringtonePreviewPlayer(new QMediaPlayer(this)),
    m_ringtonePreviewAudioOutput(new QAudioOutput(this))
{
    m_ringtonePlayer->setAudioOutput(m_ringtoneAudioOutput);
    m_ringtonePlayer->setLoops(QMediaPlayer::Infinite);

    m_volumeAnimation->setTargetObject(m_ringtoneAudioOutput);
    m_volumeAnimation->setPropertyName("volume");

    m_ringtonePreviewPlayer->setAudioOutput(m_ringtonePreviewAudioOutput);
    m_ringtonePreviewPlayer->setLoops(QMediaPlayer::Once);

    connect(m_ringtonePlayer, &QMediaPlayer::errorOccurred, this, &ClockAlarmAudio::handleRingtoneError);
    connect(m_ringtonePlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        qTrace() << "[ClockAlarmAudio] Alarm ringtone media status" << status << m_ringtonePlayer->source();
    });
    connect(m_ringtonePlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        qTrace() << "[ClockAlarmAudio] Alarm ringtone playback state" << state << m_ringtonePlayer->source();
    });
    connect(m_ringtonePlayer, &QMediaPlayer::durationChanged, this, [](qint64 duration) {
        qTrace() << "[ClockAlarmAudio] Alarm ringtone duration" << duration << "ms";
    });
    connect(m_ringtonePreviewPlayer, &QMediaPlayer::errorOccurred, this, &ClockAlarmAudio::handleRingtoneError);
    connect(m_ringtonePreviewPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if(status == QMediaPlayer::EndOfMedia)
            setRingtonePreviewing(false);
    });
}

bool ClockAlarmAudio::init()
{
    m_ringtoneAudioOutput->setVolume(m_defaultVolume / 100.0);
    qTrace() << "[ClockAlarmAudio] Initialized with default volume" << m_defaultVolume;
    return true;
}

void ClockAlarmAudio::startAlarm(AlarmObject* alarmObject)
{
    if(!alarmObject)
    {
        qWarning() << "[ClockAlarmAudio] Alarm audio start ignored: null alarm object";
        return;
    }

    stopAlarm();
    stopRingtonePreview();

    const QVariantMap details = alarmObject->getDetails();
    const int audioSource = qVariantGetNestedValue(details, "audio.source").toInt();
    const QVariant volumeValue = qVariantGetNestedValue(details, "audio.volume");
    const int volume = qBound(0, volumeValue.isValid() ? volumeValue.toInt() : m_defaultVolume, 100);
    const int fadeInDuration = qMax(0, qVariantGetNestedValue(details, "audio.fadeInDuration").toInt());

    qTrace() << "[ClockAlarmAudio] Starting audio for alarm" << alarmObject->getUuid()
            << "source" << audioSource << "volume" << volume << "fade-in" << fadeInDuration << "seconds";

    if(audioSource != ClockAlarmAudioSources::Ringtone)
    {
        m_playbackSource = PlaybackSource::MediaPlayer;
        const QVariant mediaStartModeValue = qVariantGetNestedValue(details, "audio.mediaStartMode");
        const int mediaStartMode = mediaStartModeValue.isValid() ? mediaStartModeValue.toInt() : ClockAlarmMediaStartModes::Next;
        qTrace() << "[ClockAlarmAudio] Starting media player" << "mode" << mediaStartMode;
        ClockMedia::Get()->startMedia(volume, fadeInDuration, ClockAlarmMediaStartModes::fromInt(mediaStartMode));
        return;
    }

    m_playbackSource = PlaybackSource::Ringtone;
    const QString ringtone = qVariantGetNestedValue(details, "audio.ringtone").toString();
    if(ringtone.isEmpty())
    {
        qWarning() << "[ClockAlarmAudio] No ringtone selected for alarm" << alarmObject->getUuid();
        m_playbackSource = PlaybackSource::None;
        return;
    }

    m_volumeAnimation->stop();
    if(fadeInDuration > 0)
    {
        qTrace() << "[ClockAlarmAudio] Applying ringtone fade-in" << fadeInDuration << "seconds";
        m_ringtoneAudioOutput->setVolume(0.0);
        m_volumeAnimation->setDuration(fadeInDuration * 1000);
        m_volumeAnimation->setStartValue(0.0);
        m_volumeAnimation->setEndValue(volume / 100.0);
        m_volumeAnimation->start();
    }
    else
    {
        m_ringtoneAudioOutput->setVolume(volume / 100.0);
    }

    qTrace() << "[ClockAlarmAudio] Playing ringtone" << ringtone << "at volume" << m_ringtoneAudioOutput->volume();
    m_ringtonePlayer->setSource(QUrl(ringtone));
    m_ringtonePlayer->play();
}

void ClockAlarmAudio::stopAlarm()
{
    qTrace() << "[ClockAlarmAudio] Stopping alarm audio" << static_cast<int>(m_playbackSource);
    switch (m_playbackSource) {
    case PlaybackSource::Ringtone:
        m_ringtonePlayer->stop();
        m_volumeAnimation->stop();
        break;
    case PlaybackSource::MediaPlayer:
        ClockMedia::Get()->stopMedia();
        break;
    case PlaybackSource::None:
        break;
    }

    m_playbackSource = PlaybackSource::None;
}

void ClockAlarmAudio::playRingtonePreview(const QString& ringtone, int volume)
{
    if(ringtone.isEmpty())
    {
        stopRingtonePreview();
        return;
    }

    m_ringtonePreviewPlayer->stop();
    m_ringtonePreviewAudioOutput->setVolume(qBound(0, volume, 100) / 100.0);
    m_ringtonePreviewPlayer->setSource(QUrl(ringtone));
    m_ringtonePreviewPlayer->play();
    setRingtonePreviewing(true);
    qTrace() << "[ClockAlarmAudio] Playing ringtone preview" << ringtone << "at volume" << volume;
}

void ClockAlarmAudio::stopRingtonePreview()
{
    if(m_ringtonePreviewing) {
        qTrace() << "[ClockAlarmAudio] Stopping ringtone preview";
    }
    m_ringtonePreviewPlayer->stop();
    setRingtonePreviewing(false);
}

void ClockAlarmAudio::handleRingtoneError(QMediaPlayer::Error error, const QString& errorString)
{
    QMediaPlayer* player = qobject_cast<QMediaPlayer*>(sender());
    const QString playerType = player == m_ringtonePlayer ? QStringLiteral("alarm ringtone") : QStringLiteral("ringtone preview");
    qWarning() << "[ClockAlarmAudio]" << playerType << "error" << error << errorString
               << "source" << (player ? player->source() : QUrl());
}
