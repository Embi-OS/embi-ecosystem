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

QStringList Utils::fontFamilies() const
{
    return QFontDatabase::families();
}
