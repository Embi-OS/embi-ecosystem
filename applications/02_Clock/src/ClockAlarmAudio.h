#ifndef CLOCKALARMAUDIO_H
#define CLOCKALARMAUDIO_H

#include <Axion>

#include <QMediaPlayer>

Q_ENUM_CLASS(ClockAlarmAudioSources, ClockAlarmAudioSource,
             MediaPlayer,
             Ringtone)

Q_ENUM_CLASS(ClockAlarmMediaStartModes, ClockAlarmMediaStartMode,
             Current,
             Next)

class AlarmObject;
class QAudioOutput;
class QPropertyAnimation;

class ClockAlarmAudio : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(ClockAlarmAudio)

    Q_WRITABLE_VAR_PROPERTY(int, defaultVolume, DefaultVolume, 20)
    Q_READONLY_VAR_PROPERTY(bool, ringtonePreviewing, RingtonePreviewing, false)

private:
    enum class PlaybackSource
    {
        None,
        Ringtone,
        MediaPlayer
    };

    explicit ClockAlarmAudio(QObject* parent = nullptr);

public:
    bool init() final override;

public slots:
    void startAlarm(AlarmObject* alarmObject);
    void stopAlarm();
    void playRingtonePreview(const QString& ringtone, int volume);
    void stopRingtonePreview();

private slots:
    void handleRingtoneError(QMediaPlayer::Error error, const QString& errorString);

private:
    QMediaPlayer* m_ringtonePlayer=nullptr;
    QAudioOutput* m_ringtoneAudioOutput=nullptr;
    QPropertyAnimation* m_volumeAnimation=nullptr;
    QMediaPlayer* m_ringtonePreviewPlayer=nullptr;
    QAudioOutput* m_ringtonePreviewAudioOutput=nullptr;
    PlaybackSource m_playbackSource=PlaybackSource::None;
};

#endif // CLOCKALARMAUDIO_H
