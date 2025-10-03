#ifndef A00_LAUNCHERMANAGER_H
#define A00_LAUNCHERMANAGER_H

#include <Axion>

class A00_LauncherManager: public MainManagerContainer
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit A00_LauncherManager(QObject *parent = nullptr);

    bool init() final override;
};

#endif // A00_LAUNCHERMANAGER_H
