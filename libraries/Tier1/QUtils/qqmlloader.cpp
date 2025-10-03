#include "qqmlloader.h"
#include "qutils_log.h"

#include <QThreadStorage>

Q_GLOBAL_STATIC(QThreadStorage<QQmlEngine*>, g_engines)

QQmlLoader::QQmlLoader(QObject* parent):
    QObject(parent)
{

}

QQmlLoader* QQmlLoader::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    return new QQmlLoader(jsEngine);
}

QObject* QQmlLoader::load(const QString& url, QObject* parent)
{
    return load(url, QVariantMap(), parent);
}

QObject* QQmlLoader::load(const QString& url, const QVariantMap& initialProperty, QObject* parent)
{
    QQmlEngine* engine = nullptr;

    if(parent)
        engine = qmlEngine(parent);

    if(!engine) {
        if(!g_engines->hasLocalData())
            g_engines->setLocalData(new QQmlEngine);
        engine = g_engines->localData();
    }

    QQmlComponent compo(engine, QUrl(url), QQmlComponent::PreferSynchronous, parent);
    return createObject(&compo, initialProperty, parent);
}

QObject* QQmlLoader::load(const QString& module, const QString& name, QObject* parent)
{
    return load(module, name, QVariantMap(), parent);
}

QObject* QQmlLoader::load(const QString& module, const QString& name, const QVariantMap& initialProperty, QObject* parent)
{
    QQmlEngine* engine = nullptr;

    if(parent)
        engine = qmlEngine(parent);

    if(!engine) {
        if(!g_engines->hasLocalData())
            g_engines->setLocalData(new QQmlEngine);
        engine = g_engines->localData();
    }

    QQmlComponent compo(engine, module, name, QQmlComponent::PreferSynchronous, parent);

    return createObject(&compo, initialProperty, parent);
}

QObject* QQmlLoader::createObject(QQmlComponent* compo, QObject* parent)
{
    return createObject(compo, QVariantMap(), parent);
}

QObject* QQmlLoader::createObject(QQmlComponent* compo, const QVariantMap& initialProperty, QObject* parent)
{
    QElapsedTimer timer;
    timer.start();

    if(compo->isReady())
    {
        if(QObject* obj = compo->createWithInitialProperties(initialProperty, qmlContext(parent)))
        {
            obj->QObject::setParent(parent);
            QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);

            QUTILSLOG_DEBUG()<<"Loaded:"<<compo->url()<<"into:"<<obj<<"in"<<timer.nsecsElapsed()/1000000.0<<"ms";
            return obj;
        }
    }
    else
    {
        const QList<QQmlError> errors = compo->errors();
        for(const QQmlError& qmlError: errors)
        {
            QUTILSLOG_CRITICAL()<<qmlError.toString();
        }
    }
    return nullptr;
}
