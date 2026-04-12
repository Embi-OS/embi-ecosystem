#ifndef POWERBACKENDAPALISIMX8_H
#define POWERBACKENDAPALISIMX8_H

#include "powerbackendb2qt.h"

class PowerBackendApalisIMX8 : public PowerBackendB2Qt
{
public:
    explicit PowerBackendApalisIMX8(QObject* parent=nullptr);

    int getCapabilities() override;

    void suspend(bool deep=false) final override;
    void wakeIn(int second) final override;
    bool isAlwaysOn() final override;
};

#endif // POWERBACKENDAPALISIMX8_H
