#include "A01_GalleryManager.h"

#include "Gallery.h"

A01_GalleryManager::A01_GalleryManager(QObject *parent) :
    MainManagerContainer(parent)
{

}

bool A01_GalleryManager::init()
{
    MainManagerContainer::init();

    registerManager(Gallery::Get());

    return true;
}
