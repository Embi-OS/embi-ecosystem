#ifndef A02_CLOCKMANAGER_H
#define A02_CLOCKMANAGER_H

#include <Axion>
class A02_ClockManager: public MainManagerContainer
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit A02_ClockManager(QObject *parent = nullptr);

    bool init() final override;
};

#endif // A02_CLOCKMANAGER_H
