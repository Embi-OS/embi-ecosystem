#ifndef HAPPYSERVERWORKER_H
#define HAPPYSERVERWORKER_H

#include <QDefs>
#include "happyserver.h"

class HappyServerWorker : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_VAR_PROPERTY(bool, running, Running, false)

public:
    explicit HappyServerWorker(QObject *parent = nullptr);

public slots:
    bool start(HappyServer* server);
    bool stop();

private:
    QThread *m_thread = nullptr;
    HappyServer *m_server = nullptr;
};

#endif // HAPPYSERVERWORKER_H
