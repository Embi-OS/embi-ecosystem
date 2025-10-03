#ifndef HAPPYFULLROUTER_H
#define HAPPYFULLROUTER_H

#include "happycrudrouter.h"

class HappyFullRouter : public HappyCrudRouter
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, socketPath, SocketPath, {})

public:
    explicit HappyFullRouter(QObject *parent = nullptr);

    QVariantMap info() override;

    bool init(HappyServer* happyServer) override;

    HappyReply postObject(const QVariant& data, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders()) override;
    HappyReply putObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders()) override;
    HappyReply patchObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders()) override;
    HappyReply deleteObject(const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders()) override;

protected:
    virtual void sendSocket(const HappyReply& reply, const QVariant& argValue);
};

#endif // HAPPYFULLROUTER_H
