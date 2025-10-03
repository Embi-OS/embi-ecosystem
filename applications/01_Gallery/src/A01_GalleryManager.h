#ifndef A01_GALLERYMANAGER_H
#define A01_GALLERYMANAGER_H

#include <Axion>

class A01_GalleryManager: public MainManagerContainer
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit A01_GalleryManager(QObject *parent = nullptr);

    bool init() final override;
};

#endif // A01_GALLERYMANAGER_H
