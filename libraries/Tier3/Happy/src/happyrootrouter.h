#ifndef HAPPYROOTROUTER_H
#define HAPPYROOTROUTER_H

#include <Sql>

#include "happyrouter.h"

class HappyRootRouter : public HappyRouter
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, socketPath, SocketPath, {})

public:
    explicit HappyRootRouter(QObject *parent = nullptr);

    bool init(HappyServer* happyServer) override;

    virtual HappyReply getRoot();
    virtual HappyReply getBase();

    QHttpServerResponse getRootRoute(const HappyHttpRequest &request);
    QHttpServerResponse getBaseRoute(const HappyHttpRequest &request);
};

#endif // HAPPYROOTROUTER_H
