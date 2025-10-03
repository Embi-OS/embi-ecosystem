#include "randomutils.h"

#include <QRandomGenerator>

RandomUtils::RandomUtils(QObject *parent) :
    QObject(parent)
{

}

QColor RandomUtils::color()
{
    return QColor::fromRgb(QRandomGenerator::global()->generate());
}

QString RandomUtils::uuid()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
