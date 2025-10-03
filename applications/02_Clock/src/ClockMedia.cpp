#include "ClockMedia.h"
#include "ClockDisplay.h"
#include <QStandardPaths>

ClockMedia::ClockMedia(QObject *parent) :
    AbstractManager(parent),
    m_volumeAnimation(new QPropertyAnimation(this))
{
    m_volumeAnimation->setPropertyName("volume");
    m_volumeAnimation->setDuration(5000);
    m_volumeAnimation->setEasingCurve(QEasingCurve::Linear);
    m_volumeAnimation->setStartValue(0.0);
    m_volumeAnimation->setEndValue(1.0);

    connect(this, &ClockMedia::mediaPlayerAboutToChange, this, &ClockMedia::onMediaPlayerAboutToChange);
    connect(this, &ClockMedia::mediaPlayerChanged, this, &ClockMedia::onMediaPlayerChanged);
}

bool ClockMedia::init()
{
    connect(ClockDisplay::Get(), &ClockDisplay::stateChanged, this, [this](ClockDisplayState state){
        if(state==ClockDisplayStates::On) {
            if(m_volumeAnimation->state()!=QAbstractAnimation::Running)
                return;

            m_volumeAnimation->stop();
            if(m_mediaPlayer->audioOutput()->volume()>m_defaultVolume/100.0)
                m_mediaPlayer->audioOutput()->setVolume(m_defaultVolume/100.0);
        }
    });

    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    persistantData->mapProperty(this,"wakeTimeout");
    persistantData->mapProperty(this,"defaultVolume");

    return true;
}

void ClockMedia::startMedia()
{
    if(!m_mediaPlayer)
        return;

    MediaItemModelAttached::wrap(m_mediaPlayer)->getPlaylist()->autoNext();

    if(ClockDisplay::Get()->getState()==ClockDisplayStates::Waking)
    {
        m_volumeAnimation->setDuration(m_wakeTimeout * 60 * 1000);
        m_volumeAnimation->start();
    }

    QMetaObject::invokeMethod(m_mediaPlayer, &QMediaPlayer::play, Qt::QueuedConnection);
}

void ClockMedia::stopMedia()
{
    if(!m_mediaPlayer)
        return;

    m_mediaPlayer->stop();
    m_volumeAnimation->stop();
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

    connect(mediaPlayer, &QMediaPlayer::errorOccurred, this, &ClockMedia::handleMediaPlayerError);

    m_volumeAnimation->setTargetObject(mediaPlayer->audioOutput());
}

void ClockMedia::handleMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    qWarning()<<error<<errorString;
}
