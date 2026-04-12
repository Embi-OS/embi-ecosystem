#ifndef POWERBACKENDB2QT_H
#define POWERBACKENDB2QT_H

#include "powerbackend.h"

class PowerBackendB2Qt : public PowerBackend
{
public:
    explicit PowerBackendB2Qt(QObject* parent=nullptr);

    int getCapabilities() override;

    void quit() final override;
    void restart() final override;
    void shutdown() final override;
    void reboot() final override;
};

#endif // POWERBACKENDB2QT_H
