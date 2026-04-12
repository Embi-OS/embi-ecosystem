#ifndef RESTMANAGER_H
#define RESTMANAGER_H

#include <Axion>
#include <Rest>

class RestManager : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(RestManager)

    Q_CONSTANT_PTR_PROPERTY(RestClient, client)

    Q_WRITABLE_VAR_PROPERTY(RestDataModes::Enum, apiDataMode, ApiDataMode, RestDataModes::Json)
    Q_WRITABLE_REF_PROPERTY(QString, apiBaseUrl, ApiBaseUrl, "localhost")
    Q_WRITABLE_VAR_PROPERTY(int, apiPort, ApiPort, 32768)
    Q_WRITABLE_VAR_PROPERTY(bool, apiTrailingSlash, ApiTrailingSlash, true)
    Q_WRITABLE_VAR_PROPERTY(bool, apiSocketEnabled, ApiSocketEnabled, true)
    Q_WRITABLE_REF_PROPERTY(QString, apiKey, ApiKey, "")

private:
    explicit RestManager(QObject *parent = nullptr);

public:
    bool init() final override;

    bool localhost() const;

public slots:
    void authenticate(const QString& identifier, const QString& password);

private:
    bool m_isLocalhost=false;
};

#endif // RESTMANAGER_H
