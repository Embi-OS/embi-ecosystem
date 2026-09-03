#ifndef RESTBEARERAUTHENTICATIONEXTENDER_H
#define RESTBEARERAUTHENTICATIONEXTENDER_H

#include "restrequestbuilder.h"

class RestBearerAuthenticationExtender : public RestRequestBuilderExtender
{
public:
    explicit RestBearerAuthenticationExtender(const QString &token = {});

    void setToken(const QString &token);
    QString token() const;

    void extendRequest(QNetworkRequest &request, QByteArray &verb, QByteArray *body) const override;

private:
    QString m_token;
};

#endif // RESTBEARERAUTHENTICATIONEXTENDER_H
