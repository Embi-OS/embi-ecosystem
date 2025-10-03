#ifndef HAPPYROUTER_H
#define HAPPYROUTER_H

#include <QDefs>
#include <QHttpServer>

#include <Rest>

#include "happyreply.h"
#include "happyhttprequest.h"
#include "happyhttpparameters.h"
#include "happyhttpheaders.h"

class HappyServer;
class HappyRouter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_READONLY_VAR_PROPERTY(bool, registered, Registered, false)
    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)

    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_VAR_PROPERTY(bool, route, Route, true)

public:
    explicit HappyRouter(QObject *parent = nullptr);

    virtual QVariantMap info();

    virtual bool init(HappyServer* happyServer);

signals:
    void requestReceived(const HappyHttpRequest& request);

protected:
    HappyServer* m_happyServer;
};

#endif // HAPPYROUTER_H
