#ifndef HASSSOCKETATTACHED_H
#define HASSSOCKETATTACHED_H

#include <QDefs>
#include <QMetaObject>
#include <QPointer>

#include "hasssocket.h"

class HassEntityModel;
class HassEntityMapper;

class HassSocketAttached: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    QML_ATTACHED(HassSocketAttached)

    Q_WRITABLE_PTR_PROPERTY(HassSocket, socket, Socket, nullptr)

protected:
    explicit HassSocketAttached(QObject* parent=nullptr);

public:
    static HassSocketAttached* wrap(QObject* object);
    static HassSocketAttached* qmlAttachedProperties(QObject* object);

protected slots:
    void onSocketAboutToChange(HassSocket* oldSocket, HassSocket* newSocket);
    void onSocketChanged(HassSocket* socket);

    virtual void onEntityStateChanged(const QString& entityId, const QVariantMap& state) = 0;
};

class HassSocketModelAttached: public HassSocketAttached
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

private:
    explicit HassSocketModelAttached(QObject* parent=nullptr);

protected slots:
    void onEntityStateChanged(const QString& entityId, const QVariantMap& state) final override;

private:
    friend HassSocketAttached;
    HassEntityModel* m_model;
};

class HassSocketMapperAttached: public HassSocketAttached
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

private:
    explicit HassSocketMapperAttached(QObject* parent=nullptr);

protected slots:
    void onEntityStateChanged(const QString& entityId, const QVariantMap& state) final override;

private:
    friend HassSocketAttached;
    HassEntityMapper* m_mapper;
};

#endif // HASSSOCKETATTACHED_H
