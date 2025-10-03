#include "snackbarobject.h"

SnackbarObject::SnackbarObject(const QVariant& settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_isMarkedForDeletion(false)
{
    if(settings.canConvert<QString>())
    {
        m_caption = settings.toString();
    }
    else
    {
        const QVariantMap map = settings.toMap();
        if(map.contains("severity"))
            m_severity = (SnackbarSeverity)map.value("severity").toInt();
        if(map.contains("title"))
            m_title = map.value("title").toString();
        if(map.contains("caption"))
            m_caption = map.value("caption").toString();
        if(map.contains("details"))
            m_details = map.value("details").toString();
        if(map.contains("button"))
            m_button = map.value("button").toString();
        if(map.contains("closable"))
            m_closable = map.value("closable").toBool();
        if(map.contains("timeout"))
            m_timeout = map.value("timeout").toInt();

        if(map.contains("diagnose"))
            m_diagnose = map.value("diagnose").toBool();
    }

    connect(this, &SnackbarObject::buttonChanged, this, &SnackbarObject::updateTimeout);
    connect(this, &SnackbarObject::closableChanged, this, &SnackbarObject::updateTimeout);
    connect(this, &SnackbarObject::timeoutChanged, this, &SnackbarObject::updateTimeout);
    connect(this, &SnackbarObject::visibleChanged, this, &SnackbarObject::updateTimeout);

    updateTimeout();
}

void SnackbarObject::updateTimeout()
{
    if(m_progressAnimation)
    {
        m_progressAnimation->stop();
        m_progressAnimation->deleteLater();
        m_progressAnimation = nullptr;
    }

    if(m_visible && m_timeout>0 && !m_closable && m_button.isEmpty())
    {
        m_progressAnimation = new QPropertyAnimation(this, "progress", this);
        m_progressAnimation->setDuration(m_timeout);
        m_progressAnimation->setStartValue(10000);
        m_progressAnimation->setEndValue(0);
        m_progressAnimation->start();

        connect(m_progressAnimation, &QPropertyAnimation::finished, this, [this](){
            m_progressAnimation->deleteLater();
            close();
        }, Qt::QueuedConnection);
    }
}

QMetaObject::Connection SnackbarObject::onClosed(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &SnackbarObject::closed, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection SnackbarObject::onAccepted(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &SnackbarObject::accepted, this, [callback](){
        callback();
    }, connection);
}

void SnackbarObject::close()
{
    emit this->closed();

    markForDeletion();
}

void SnackbarObject::accept()
{
    emit this->accepted();

    markForDeletion();
}

void SnackbarObject::markForDeletion()
{
    m_isMarkedForDeletion=true;
    deleteLater();
}

bool SnackbarObject::isMarkedForDeletion() const
{
    return m_isMarkedForDeletion;
}
