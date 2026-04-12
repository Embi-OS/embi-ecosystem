#ifndef POWERBACKENDDESKTOP_H
#define POWERBACKENDDESKTOP_H

#include "powerbackend.h"

class PowerBackendDesktop : public PowerBackend
{
public:
    explicit PowerBackendDesktop(QObject* parent=nullptr);

    int getCapabilities() override;

    void quit() override;
    void restart() override;
};

#endif // POWERBACKENDDESKTOP_H
