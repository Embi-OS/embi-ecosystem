#include "hasssocketattached.h"

#include "hassentitymapper.h"
#include "hassentitymodel.h"

#include "syncable/qsdiffrunner.h"
#include <QQmlEngine>

HassSocketAttached::HassSocketAttached(QObject* parent):
    QObject(parent)
{
    connect(this, &HassSocketAttached::socketAboutToChange, this, &HassSocketAttached::onSocketAboutToChange);
    connect(this, &HassSocketAttached::socketChanged, this, &HassSocketAttached::onSocketChanged);
}

void HassSocketAttached::onSocketAboutToChange(HassSocket* oldSocket, HassSocket* newSocket)
{
    if(!oldSocket)
        return;

    disconnect(oldSocket, nullptr, this, nullptr);
    disconnect(this, nullptr, oldSocket, nullptr);
}

void HassSocketAttached::onSocketChanged(HassSocket* socket)
{
    if(!socket)
        return;

    connect(socket, &HassSocket::entityStateChanged, this, &HassSocketAttached::onEntityStateChanged);
}

HassSocketAttached* HassSocketAttached::wrap(QObject* object)
{
    HassEntityModel* model = qobject_cast<HassEntityModel*>(object);
    HassEntityMapper* mapper = qobject_cast<HassEntityMapper*>(object);
    if(!model && !mapper) {
        qCritical("HassSocketAttached can only be attached to a HassEntityModel or HassEntityMapper");
        return nullptr;
    }

    HassSocketAttached* attached = object->findChild<HassSocketAttached*>(QString(), Qt::FindDirectChildrenOnly);
    if(attached)
        return attached;

    if(model)
        attached = new HassSocketModelAttached(object);
    else
        attached = new HassSocketMapperAttached(object);
    QQmlEngine::setObjectOwnership(attached, QQmlEngine::CppOwnership);
    return attached;
}

HassSocketAttached* HassSocketAttached::qmlAttachedProperties(QObject* object)
{
    return wrap(object);
}

HassSocketModelAttached::HassSocketModelAttached(QObject* parent):
    HassSocketAttached(parent),
    m_model(qobject_cast<HassEntityModel*>(parent))
{}

void HassSocketModelAttached::onEntityStateChanged(const QString& entityId, const QVariantMap& state)
{
    if(!m_model->getSyncable()) {
        m_model->queueSelect();
        return;
    }

    const QString& primaryField = m_model->getPrimaryField();
    const int index = m_model->indexOf(primaryField, entityId,
                                       primaryField == m_model->getSortRoleName(),
                                       m_model->getSortOrder());
    if(state.isEmpty()) {
        if(index >= 0)
            m_model->remove(index);
        return;
    }

    if(index >= 0) {
        m_model->set(index, state);
        return;
    }

    int insertIndex = m_model->size();
    if(m_model->getSortEnabled()) {
        insertIndex = QModelHelper::indexOf(m_model, m_model->getSortRoleName(),
                                            state.value(m_model->getSortRoleName()),
                                            true, m_model->getSortOrder(), true);
    }
    m_model->insert(insertIndex, state);
}

HassSocketMapperAttached::HassSocketMapperAttached(QObject* parent):
    HassSocketAttached(parent),
    m_mapper(qobject_cast<HassEntityMapper*>(parent))
{}

void HassSocketMapperAttached::onEntityStateChanged(const QString& entityId, const QVariantMap& state)
{
    if(entityId != m_mapper->getMethod())
        return;

    if(!m_mapper->getSyncable()) {
        m_mapper->queueSelect();
        return;
    }

    if(state.isEmpty()) {
        m_mapper->alter({});
        m_mapper->setExists(false);
        return;
    }

    const QVariantMap diff = QSDiffRunner::compare(m_mapper->getStorage(), state);
    m_mapper->alter(diff);
    m_mapper->setExists(true);
}
