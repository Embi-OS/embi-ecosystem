#include "restsocketattached.h"
#include "rest_log.h"

#include "syncable/qsdiffrunner.h"

#include <QQmlProperty>

RestSocketAttached::RestSocketAttached(QObject* parent):
    RestSocket(parent)
{
    setDelayed(true);
    setEnabled(false);

    connect(this, &RestSocket::messageReceived, this, &RestSocketAttached::onSocketMessageReceived);
    connect(this, &RestSocket::error, this, &RestSocketAttached::onSocketError);
}

RestSocketAttached* RestSocketAttached::wrap(QObject* object)
{
    RestModel* model = qobject_cast<RestModel*>(object);
    RestMapper* mapper = qobject_cast<RestMapper*>(object);
    if (!model && !mapper)
    {
        QMODELSLOG_CRITICAL()<<object;
        QMODELSLOG_FATAL("RestSocket can only be attached to a RestModel* or RestMapper*");
        return nullptr;
    }

    RestSocketAttached* attached = object->findChild<RestSocketAttached*>(QString(), Qt::FindDirectChildrenOnly);
    if(attached)
        return attached;

    if(model)
        attached = new RestSocketModelAttached(object);
    else if(mapper)
        attached = new RestSocketMapperAttached(object);
    QQmlEngine::setObjectOwnership(attached, QQmlEngine::CppOwnership);

    return attached;
}

RestSocketAttached* RestSocketAttached::qmlAttachedProperties(QObject* object)
{
    return wrap(object);
}

void RestSocketAttached::changeBaseName(const QString& baseName)
{
    if(baseName.isEmpty())
        return;
    QString path = baseName;
    path.replace("api/", "ws/");
    setPath(path);
}

RestSocketModelAttached::RestSocketModelAttached(QObject* parent):
    RestSocketAttached(parent),
    m_model(qobject_cast<RestModel*>(parent))
{
    if (!m_model)
    {
        RESTLOG_CRITICAL()<<parent;
        RESTLOG_FATAL("RestSocketModelAttached can only be parented to a RestModel*");
    }

    connect(m_model, &RestModel::connectionChanged, this, &RestSocket::setConnection);
    connect(m_model, &RestModel::baseNameChanged, this, &RestSocketModelAttached::changeBaseName);

    setConnection(m_model->getConnection());
    changeBaseName(m_model->getBaseName());

    if(m_model->getSelected())
        bind();
    else
        connect(m_model, &RestModel::selectDone, this, &RestSocket::bind, Qt::SingleShotConnection);
}

void RestSocketModelAttached::onSocketMessageReceived(const QVariant& message)
{
    if(m_model->getLoading())
        return;

    if(m_model->getSyncable())
        processSocketMessage(message);
    else
        m_model->queueSelect();
}

void RestSocketModelAttached::onSocketError()
{
    RESTLOG_CRITICAL()<<"Socket error"<<getUrl()<<getError();
    m_model->setError("Socket error");
    m_model->setMessage(getError());
    emit m_model->error();
}

void RestSocketModelAttached::processSocketMessage(const QVariant& message)
{
    const QVariantMap event = message.toMap();
    const QString action = event.value("action").toString();
    const QString ressource = event.value("ressource").toString();
    const QVariantMap data = event.value("data").toMap();

    if(m_model->getFilterEnabled())
    {
        const QVariant filterValue = data.value(m_model->getFilterRoleName());
        if(filterValue!=m_model->getFilterValue())
            return;
        for(auto [key, value]: m_model->getFilterMap().asKeyValueRange()) {
            const QVariant filterValue = data.value(key);
            if(filterValue!=value)
                return;
        }
    }

    RESTLOG_DEBUG()<<action<<"socket received on ressource"<<ressource;
    RESTLOG_TRACE().noquote()<<QUtils::Log::variantToLog(message);

    QSPatch patch;
    if(action=="UPDATE" || action=="UPGRADE")
    {
        int index = m_model->indexOf(m_model->getPrimaryField(), ressource, m_model->getPrimaryField()==m_model->getSortRoleName(), m_model->getSortOrder(), false);
        if(index<0)
            return;

        const QVariantMap diff = QSDiffRunner::compare(m_model->at(index).toMap(), data);
        patch = QSPatch::createUpdate(index, diff);
        emit this->updateReceived(diff);
    }
    else if(action=="CREATE")
    {
        int index = m_model->indexOf(m_model->getPrimaryField(), ressource, m_model->getPrimaryField()==m_model->getSortRoleName(), m_model->getSortOrder(), false);
        if(index>=0)
            return;

        index = m_model->size();
        if(m_model->getSortEnabled())
        {
            const QVariant sortValue = data.value(m_model->getSortRoleName());
            index = QModelHelper::indexOf(m_model, m_model->getSortRoleName(), sortValue, true, m_model->getSortOrder(), true);
        }
        patch = QSPatch::createInsert(index, data);
        emit this->createReceived(data);
    }
    else if(action=="DELETE")
    {
        int index = m_model->indexOf(m_model->getPrimaryField(), ressource, m_model->getPrimaryField()==m_model->getSortRoleName(), m_model->getSortOrder(), false);
        if(index<0)
            return;

        const QVariantMap data = m_model->at(index).toMap();
        patch = QSPatch::createRemove(index, index);
        emit this->deleteReceived(data);
    }
    else
        return;

    m_model->alter(patch);
}

RestSocketMapperAttached::RestSocketMapperAttached(QObject* parent):
    RestSocketAttached(parent),
    m_mapper(qobject_cast<RestMapper*>(parent))
{
    if (!m_mapper)
    {
        RESTLOG_CRITICAL()<<parent;
        RESTLOG_FATAL("RestSocketMapperAttached can only be parented to a RestMapper*");
    }

    connect(m_mapper, &RestMapper::connectionChanged, this, &RestSocket::setConnection);
    connect(m_mapper, &RestMapper::methodChanged, this, &RestSocket::setMethod);
    connect(m_mapper, &RestMapper::baseNameChanged, this, &RestSocketMapperAttached::changeBaseName);

    setConnection(m_mapper->getConnection());
    setMethod(m_mapper->getMethod());
    changeBaseName(m_mapper->getBaseName());

    if(m_mapper->getSelected())
        bind();
    else
        connect(m_mapper, &RestMapper::selectDone, this, &RestSocket::bind, Qt::SingleShotConnection);
}

void RestSocketMapperAttached::onSocketMessageReceived(const QVariant& message)
{
    if(m_mapper->getLoading())
        return;

    if(m_mapper->getSyncable())
        processSocketMessage(message);
    else
        m_mapper->queueSelect();
}

void RestSocketMapperAttached::onSocketError()
{
    RESTLOG_CRITICAL()<<"Socket error"<<getUrl()<<getError();
    m_mapper->setError("Socket error");
    m_mapper->setMessage(getError());
    emit m_mapper->error();
}

void RestSocketMapperAttached::processSocketMessage(const QVariant& message)
{
    const QVariantMap event = message.toMap();
    const QString action = event.value("action").toString();
    const QString ressource = event.value("ressource").toString();

    if(ressource!=m_mapper->getMethod())
        return;

    RESTLOG_DEBUG()<<action<<"socket received";
    RESTLOG_TRACE().noquote()<<QUtils::Log::variantToLog(message);

    if(action=="UPDATE" || action=="UPGRADE")
    {
        const QVariantMap data = event.value("data").toMap();
        const QVariantMap diff = QSDiffRunner::compare(m_mapper->getStorage(), data);
        m_mapper->alter(diff);

        emit this->updateReceived(event);
    }
    else if(action=="CREATE")
    {
        emit this->createReceived(event);
    }
    else if(action=="DELETE")
    {
        emit this->deleteReceived(event);

        if(m_autoDelete)
            m_mapper->deleteLater();
    }
}
