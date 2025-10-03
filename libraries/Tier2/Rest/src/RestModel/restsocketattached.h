#ifndef RESTSOCKETATTACHED_H
#define RESTSOCKETATTACHED_H

#include <QDefs>
#include "restmodel.h"
#include "restmapper.h"
#include "restsocket.h"

class RestSocketAttached : public RestSocket
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    QML_ATTACHED(RestSocketAttached)

protected:
    explicit RestSocketAttached(QObject* parent=nullptr);

public:
    static RestSocketAttached* wrap(QObject* object);
    static RestSocketAttached* qmlAttachedProperties(QObject* object);

protected slots:
    void changeBaseName(const QString& baseName);
    virtual void onSocketMessageReceived(const QVariant& message) = 0;
    virtual void onSocketError() = 0;

signals:
    void createReceived(const QVariantMap& event);
    void updateReceived(const QVariantMap& event);
    void deleteReceived(const QVariantMap& event);
};

class RestSocketModelAttached : public RestSocketAttached
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

private:
    explicit RestSocketModelAttached(QObject* parent=nullptr);

protected slots:
    void onSocketMessageReceived(const QVariant& message) final override;
    void onSocketError() final override;

protected:
    void processSocketMessage(const QVariant& message);

private:
    friend RestSocketAttached;
    RestModel* m_model;
};

class RestSocketMapperAttached : public RestSocketAttached
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_WRITABLE_VAR_PROPERTY(bool, autoDelete, AutoDelete, false)

private:
    explicit RestSocketMapperAttached(QObject* parent=nullptr);

protected slots:
    void onSocketMessageReceived(const QVariant& message) final override;
    void onSocketError() final override;

protected:
    void processSocketMessage(const QVariant& message);

private:
    friend RestSocketAttached;
    RestMapper* m_mapper;
};

#endif // RESTSOCKETATTACHED_H
