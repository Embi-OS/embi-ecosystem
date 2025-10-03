#ifndef QQMLLOADER_H
#define QQMLLOADER_H

#include <QString>
#include <QUrl>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QElapsedTimer>

#include "qutils_log.h"

class QQmlLoader: public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(QmlLoader)
    QML_SINGLETON

private:
    explicit QQmlLoader(QObject* parent=nullptr);

public:
    static QQmlLoader* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Q_INVOKABLE static QObject* load(const QString& url, QObject* parent=nullptr);
    Q_INVOKABLE static QObject* load(const QString& url, const QVariantMap& initialProperty, QObject* parent=nullptr);
    Q_INVOKABLE static QObject* load(const QString& module, const QString& name, QObject* parent=nullptr);
    Q_INVOKABLE static QObject* load(const QString& module, const QString& name, const QVariantMap& initialProperty, QObject* parent=nullptr);

    Q_INVOKABLE static QObject* createObject(QQmlComponent* compo, QObject* parent=nullptr);
    Q_INVOKABLE static QObject* createObject(QQmlComponent* compo, const QVariantMap& initialProperty, QObject* parent=nullptr);

    template <typename T>
    static T* create(QQmlComponent* compo, QObject* parent=nullptr);
    template <typename T>
    static T* create(QQmlComponent* compo, const QVariantMap& initialProperty, QObject* parent=nullptr);

    template <typename T>
    static T* load(const QString& url, QObject* parent=nullptr);
    template <typename T>
    static T* load(const QString& url, const QVariantMap& initialProperty, QObject* parent=nullptr);
    template <typename T>
    static T* load(const QString& module, const QString& name, QObject* parent=nullptr);
    template <typename T>
    static T* load(const QString& module, const QString& name, const QVariantMap& initialProperty, QObject* parent=nullptr);

    template <typename T>
    static T* cast(QObject* object);
};

template <typename T>
T* QQmlLoader::create(QQmlComponent* compo, QObject* parent)
{
    return cast<T>(createObject(compo, parent));
}

template <typename T>
T* QQmlLoader::create(QQmlComponent* compo, const QVariantMap& initialProperty, QObject* parent)
{
    return cast<T>(createObject(compo, initialProperty, parent));
}

template <typename T>
T* QQmlLoader::load(const QString& url, QObject* parent)
{
    return cast<T>(load(url, parent));
}

template <typename T>
T* QQmlLoader::load(const QString& url, const QVariantMap& initialProperty, QObject* parent)
{
    return cast<T>(load(url, initialProperty, parent));
}

template <typename T>
T* QQmlLoader::load(const QString& module, const QString& name, QObject* parent)
{
    return cast<T>(load(module, name, parent));
}

template <typename T>
T* QQmlLoader::load(const QString& module, const QString& name, const QVariantMap& initialProperty, QObject* parent)
{
    return cast<T>(load(module, name, initialProperty, parent));
}

template <typename T>
T* QQmlLoader::cast(QObject* object)
{
    if(!object)
    {
        QUTILSLOG_WARNING()<<"No object provided";
        return nullptr;
    }

    if(T* casted = qobject_cast<T*> (object))
    {
        QUTILSLOG_DEBUG()<<object<<"casted to"<<T::staticMetaObject.className();
        return casted;
    }
    else
    {
        QUTILSLOG_CRITICAL()<<"Unable to cast"<<object<<"to"<<T::staticMetaObject.className();
    }

    return nullptr;
}

#endif // QQMLLOADER_H
