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

    Q_WRITABLE_VAR_PROPERTY(bool, localApiEnabled, LocalApiEnabled, false)

    Q_WRITABLE_VAR_PROPERTY(RestDataModes::Enum, apiDataMode, ApiDataMode, RestDataModes::Json)
    Q_WRITABLE_REF_PROPERTY(QString, apiBaseUrl, ApiBaseUrl, "localhost")
    Q_WRITABLE_VAR_PROPERTY(int, apiPort, ApiPort, 32768)
    Q_WRITABLE_VAR_PROPERTY(bool, apiTrailingSlash, ApiTrailingSlash, true)
    Q_WRITABLE_VAR_PROPERTY(bool, apiSocketEnabled, ApiSocketEnabled, true)
    Q_WRITABLE_REF_PROPERTY(QString, apiKey, ApiKey, "")

    Q_READONLY_REF_PROPERTY(QString, apiEffectiveUrl, ApiEffectiveUrl, "")

private:
    explicit RestManager(QObject *parent = nullptr);

public:
    bool init() final override;

public slots:
    void authenticate(const QString& identifier, const QString& password);
};

#endif // RESTMANAGER_H
