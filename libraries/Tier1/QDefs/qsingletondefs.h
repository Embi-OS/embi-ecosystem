#ifndef QSINGLETONDEFS_H
#define QSINGLETONDEFS_H

#include <QObject>
#include <QThreadStorage>
#include <QQmlEngine>
#include <QJSEngine>

//──────────────────────────────────────────────────────────────────────
// Private: for declaration purpose
//──────────────────────────────────────────────────────────────────────

#define _Q_OBJECT_SINGLETON_IMPL(TYPE) \
    public: \
    static TYPE* Get() \
    { \
        static QPointer<TYPE> instance = nullptr; \
        if(instance.isNull()) \
        { \
            instance = new TYPE; \
            instance->setParent(nullptr); \
            QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership); \
        } \
        return instance.data(); \
    } \
    private: \

#define _Q_OBJECT_QML_SINGLETON_IMPL(TYPE) \
    _Q_OBJECT_SINGLETON_IMPL(TYPE) \
    public: \
    static TYPE *create(QQmlEngine *engine,  QJSEngine *scriptEngine) \
    { \
        Q_UNUSED(engine); \
        Q_UNUSED(scriptEngine); \
        return Get(); \
    } \
    private: \

//──────────────────────────────────────────────────────────────────────
// Public: use these instead
//──────────────────────────────────────────────────────────────────────

#define Q_OBJECT_SINGLETON(TYPE)            _Q_OBJECT_SINGLETON_IMPL(TYPE)
#define Q_OBJECT_QML_SINGLETON(TYPE)        _Q_OBJECT_QML_SINGLETON_IMPL(TYPE)

struct _QSINGLETONDEFS_ { Q_GADGET }; // mock object

#endif // QSINGLETONDEFS_H
