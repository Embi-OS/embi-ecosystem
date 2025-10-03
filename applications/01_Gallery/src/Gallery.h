#ifndef GALLERY_H
#define GALLERY_H

#include <Axion>
class Gallery : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(Gallery)

private:
    explicit Gallery(QObject *parent = nullptr);

public:
    bool init() final override;
};

#endif // GALLERY_H
