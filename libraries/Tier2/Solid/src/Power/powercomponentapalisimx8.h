#ifndef POWERCOMPONENTAPALISIMX8_H
#define POWERCOMPONENTAPALISIMX8_H

#include "powercomponentb2qt.h"
class PowerComponentApalisIMX8 : public PowerComponentB2qt
{
public:
    PowerComponentApalisIMX8();

    int getCapabilities() override;

    void suspend(bool deep=false) final override;
    void wakeIn(int second) final override;
    bool isAlwaysOn() final override;
};

#endif // POWERCOMPONENTAPALISIMX8_H
