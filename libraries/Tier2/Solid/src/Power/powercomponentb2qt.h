#ifndef POWERCOMPONENTB2QT_H
#define POWERCOMPONENTB2QT_H

#include "abstractpowercomponent.h"
class PowerComponentB2qt : public AbstractPowerComponent
{
public:
    PowerComponentB2qt();

    int getCapabilities() override;

    void quit() final override;
    void restart() final override;
    void shutdown() final override;
    void reboot() final override;
    void launch(const QString& path) final override;
};

#endif // POWERCOMPONENTB2QT_H
