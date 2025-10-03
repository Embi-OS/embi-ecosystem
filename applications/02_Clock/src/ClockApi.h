#ifndef CLOCKAPI_H
#define CLOCKAPI_H

#include <Axion>
#include <Happy>

class ClockApi : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(ClockApi)

    Q_READONLY_VAR_PROPERTY(bool, running, Running, false)

protected:
    explicit ClockApi(QObject *parent = nullptr);

public:
    bool init() final override;
    bool unInit() final override;

private:
    HappyServerWorker* m_worker = nullptr;
};

#endif // CLOCKAPI_H
