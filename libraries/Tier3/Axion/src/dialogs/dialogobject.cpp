#include "dialogobject.h"

DialogObject::DialogObject(const QVariant& settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_isMarkedForDeletion(false)
{
    onSettingsChanged(settings);

    connect(this, &DialogObject::settingsChanged, this, &DialogObject::onSettingsChanged);
    connect(this, &DialogObject::closed, this, &DialogObject::hide);
}

void DialogObject::onSettingsChanged(const QVariant& settings)
{
    if(settings.canConvert<QString>())
    {
        setInfos(settings.toString());
    }
    else
    {
        const QVariantMap map = settings.toMap();
        if(map.contains("fullscreen"))
            setFullscreen(map.value("fullscreen").toBool());
        if(map.contains("title"))
            setTitle(map.value("title").toString());
        if(map.contains("message"))
            setMessage(map.value("message").toString());
        if(map.contains("infos"))
            setInfos(map.value("infos").toString());
        if(map.contains("traces"))
            setTraces(map.value("traces").toString());
        if(map.contains("severity"))
            setSeverity((DialogSeverity)map.value("severity").toInt());

        if(map.contains("buttonApply"))
            setButtonApply(map.value("buttonApply").toString());
        if(map.contains("buttonAccept"))
            setButtonAccept(map.value("buttonAccept").toString());
        if(map.contains("buttonReject"))
            setButtonReject(map.value("buttonReject").toString());
        if(map.contains("buttonDiscard"))
            setButtonDiscard(map.value("buttonDiscard").toString());
        if(map.contains("buttonHelp"))
            setButtonHelp(map.value("buttonHelp").toString());

        if(map.contains("diagnose"))
            setDiagnose(map.value("diagnose").toBool());
    }
}

// QtQuick.Controls Popup
QMetaObject::Connection DialogObject::onAboutToHide(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::aboutToHide, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onAboutToShow(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::aboutToShow, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onClosed(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::closed, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onOpened(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::opened, this, [callback](){
        callback();
    }, connection);
}

// QtQuick.Controls Dialog
QMetaObject::Connection DialogObject::onAccepted(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::accepted, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onApplied(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::applied, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onDiscarded(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::discarded, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onHelpRequested(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::helpRequested, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onRejected(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::rejected, this, [callback](){
        callback();
    }, connection);
}

QMetaObject::Connection DialogObject::onReset(std::function<void()> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::reset, this, [callback](){
        callback();
    }, connection);
}

// Eco.Tier3.Axion Dialog
QMetaObject::Connection DialogObject::onDateSelected(std::function<void(const QDate& date)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::dateSelected, this, [callback](const QDate& date){
        callback(date);
    }, connection);
}

QMetaObject::Connection DialogObject::onTimeSelected(std::function<void(const QTime& time)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::timeSelected, this, [callback](const QTime& time){
        callback(time);
    }, connection);
}

QMetaObject::Connection DialogObject::onPathSelected(std::function<void(const QString& path)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::pathSelected, this, [callback](const QString& path){
        callback(path);
    }, connection);
}

QMetaObject::Connection DialogObject::onInputAccepted(std::function<void(const QString& value)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::inputAccepted, this, [callback](const QString& value){
        callback(value);
    }, connection);
}

QMetaObject::Connection DialogObject::onInputApplied(std::function<void(const QString& value)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::inputApplied, this, [callback](const QString& value){
        callback(value);
    }, connection);
}

QMetaObject::Connection DialogObject::onFormValidated(std::function<void(const QVariantMap& formValues)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::formValidated, this, [callback](const QVariantMap& formValues){
        callback(formValues);
    }, connection);
}

QMetaObject::Connection DialogObject::onAlarmValidated(std::function<void(const QVariantMap& alarmMap)> callback, const Qt::ConnectionType &connection)
{
    if(!callback)
        return {};

    return connect(this, &DialogObject::alarmValidated, this, [callback](const QVariantMap& alarmMap){
        callback(alarmMap);
    }, connection);
}

void DialogObject::markForDeletion()
{
    m_isMarkedForDeletion=true;
    deleteLater();
}

bool DialogObject::isMarkedForDeletion() const
{
    return m_isMarkedForDeletion;
}

void DialogObject::show()
{
    setIsVisible(true);
}

void DialogObject::hide()
{
    setIsVisible(false);
    markForDeletion();
}

