#include "happyrouter.h"

HappyRouter::HappyRouter(QObject *parent) :
    QObject(parent)
{

}

QVariantMap HappyRouter::info()
{
    QVariantMap map;

    map.insert("ready", getReady());
    map.insert("path", getPath());

    return map;
}

bool HappyRouter::init(HappyServer* happyServer)
{
    m_happyServer = happyServer;

    return true;
}

