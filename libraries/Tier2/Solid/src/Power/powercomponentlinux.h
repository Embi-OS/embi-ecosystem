#ifndef POWERCOMPONENTLINUX_H
#define POWERCOMPONENTLINUX_H

#include "abstractpowercomponent.h"
class PowerComponentLinux : public AbstractPowerComponent
{
public:
    PowerComponentLinux();

    int getCapabilities() override;

    void quit() override;
    void restart() override;
};

#endif // POWERCOMPONENTLINUX_H
