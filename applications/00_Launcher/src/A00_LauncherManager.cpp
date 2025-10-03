#include "A00_LauncherManager.h"

#include "Launcher.h"

A00_LauncherManager::A00_LauncherManager(QObject *parent) :
    MainManagerContainer(parent)
{

}

bool A00_LauncherManager::init()
{
    MainManagerContainer::init();

    registerManager(Launcher::Get());

    return true;
}
