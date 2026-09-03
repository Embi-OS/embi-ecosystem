#include "restbearerauthenticationextender.h"

RestBearerAuthenticationExtender::RestBearerAuthenticationExtender(const QString &token) :
    m_token(token)
{

}

void RestBearerAuthenticationExtender::setToken(const QString &token)
{
    m_token = token;
}

QString RestBearerAuthenticationExtender::token() const
{
    return m_token;
}

void RestBearerAuthenticationExtender::extendRequest(QNetworkRequest &request, QByteArray &verb, QByteArray *body) const
{
    Q_UNUSED(verb)
    Q_UNUSED(body)

    if(!m_token.isEmpty())
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
}
