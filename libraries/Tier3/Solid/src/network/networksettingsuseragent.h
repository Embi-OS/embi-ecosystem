#ifndef NETWORKSETTINGSUSERAGENT_H
#define NETWORKSETTINGSUSERAGENT_H

#include <QDefs>

class NetworkSettingsUserAgentPrivate;
class NetworkSettingsUserAgent : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsUserAgent(QObject *parent = nullptr);
    Q_INVOKABLE void setPassphrase(const QString &passphrase);
    Q_INVOKABLE void cancelInput();
    QString passphrase() const;
    void setSsidAndPassphrase(const QString &ssid, const QString &passphrase);
    void clearConnectionState();

Q_SIGNALS:
    void showUserCredentialsInput();
    void error();
    void ready(bool cancel);
    void requestNextConnection();
private:
    NetworkSettingsUserAgentPrivate *d_ptr;

    Q_DISABLE_COPY(NetworkSettingsUserAgent)
    Q_DECLARE_PRIVATE(NetworkSettingsUserAgent)
};

#endif // NETWORKSETTINGSUSERAGENT_H
