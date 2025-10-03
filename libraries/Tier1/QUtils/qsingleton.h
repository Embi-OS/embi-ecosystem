#ifndef QSINGLETON_H
#define QSINGLETON_H

#include <QObject>
#include <QThreadStorage>
#include <QQmlEngine>
#include <QJSEngine>

template<class T>
class QSingleton
{
public:
    QSingleton() Q_DECL_EQ_DEFAULT;
    ~QSingleton() Q_DECL_EQ_DEFAULT;
    Q_DISABLE_COPY(QSingleton)

    static T* Get()
    {
        static QThreadStorage<T*> instances;
        if(!instances.hasLocalData())
        {
            T* instance = new T;
            instances.setLocalData(instance);
            instance->setParent(nullptr);
            QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
        }
        return instances.localData();
    }
};

template<class T>
class QQmlSingleton
{
public:
    QQmlSingleton() Q_DECL_EQ_DEFAULT;
    ~QQmlSingleton() Q_DECL_EQ_DEFAULT;
    Q_DISABLE_COPY(QQmlSingleton)

    static T* Get()
    {
        static QThreadStorage<T*> instances;
        if(!instances.hasLocalData())
        {
            T* instance = new T;
            instances.setLocalData(instance);
            instance->setParent(nullptr);
            QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
        }
        return instances.localData();
    }
    static T *create(QQmlEngine *qmlEngine,  QJSEngine *jsEngine)
    {
        T* instance = Get();
        instance->m_qmlEngine = qmlEngine;
        instance->m_jsEngine = jsEngine;
        return instance;
    }

protected:
    QQmlEngine *m_qmlEngine=nullptr;
    QJSEngine *m_jsEngine=nullptr;
};

#endif // QSINGLETON_H
