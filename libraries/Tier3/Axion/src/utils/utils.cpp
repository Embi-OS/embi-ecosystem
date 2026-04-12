#include "utils.h"
#include <QFontDatabase>
#include <QUtils>

Utils::Utils(QObject *parent) :
    QObject(parent)
{

}

void Utils::await(int msecs, QEventLoop::ProcessEventsFlags flags)
{
    QUtils::await(msecs, flags);
}

void Utils::singleshotTimer(int interval, QJSValue callback)
{
    if(!callback.isCallable())
        return;

    QTimer::singleShot(interval, [callback](){
        callback.call();
    });
}

void Utils::singleshotTimer(int interval, QObject* context, QJSValue callback)
{
    if(!context || !callback.isCallable())
        return;

    QTimer::singleShot(interval, context, [callback](){
        callback.call();
    });
}

QStringList Utils::fontFamilies()
{
    return QFontDatabase::families();
}

bool Utils::pointIsValid(const QPointF& point)
{
    return !point.isNull();
}
