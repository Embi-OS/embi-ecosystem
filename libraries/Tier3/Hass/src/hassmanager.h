#ifndef HASSMANAGER_H
#define HASSMANAGER_H

#include <Axion>
#include <Rest>

#include <QDateTime>

class HassManager : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_OBJECT_QML_SINGLETON(HassManager)

    Q_CONSTANT_PTR_PROPERTY(RestClient, client)

    Q_WRITABLE_REF_PROPERTY(QString, apiBaseUrl, ApiBaseUrl, "homeassistant.local")
    Q_WRITABLE_VAR_PROPERTY(int, apiPort, ApiPort, 8123)
    Q_WRITABLE_REF_PROPERTY(QString, apiToken, ApiToken, "")

    Q_READONLY_REF_PROPERTY(QString, apiEffectiveUrl, ApiEffectiveUrl, "")

private:
    explicit HassManager(QObject *parent = nullptr);

public:
    bool init() final override;

    Q_INVOKABLE void callService(const QString& domain,
                                 const QString& service,
                                 const QVariantMap& data);
    Q_INVOKABLE void setEntityEnabled(const QString& entityId, bool enabled);
    Q_INVOKABLE void activateScene(const QString& entityId);
    Q_INVOKABLE void setInputTime(const QString& entityId, const QTime& time);
    Q_INVOKABLE void setInputDate(const QString& entityId, const QDate& date);
    Q_INVOKABLE void setInputDateTime(const QString& entityId, const QDateTime& dateTime);
};

#endif // HASSMANAGER_H
