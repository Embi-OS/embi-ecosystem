#ifndef QATTACHEDDEFS_H
#define QATTACHEDDEFS_H

#include <QObject>
#include <QQmlEngine>

//──────────────────────────────────────────────────────────────────────
// Private: for declaration purpose
//──────────────────────────────────────────────────────────────────────

#define _Q_OBJECT_ATTACHED_IMPL(TYPE, PARENT_TYPE) \
    public: \
    static TYPE* wrap(QObject* object) \
    { \
        return qobject_cast<TYPE*>(qmlAttachedPropertiesObject<TYPE>(object, true)); \
    } \
    static TYPE* qmlAttachedProperties(QObject* object) \
    { \
        PARENT_TYPE* parent = qobject_cast<PARENT_TYPE*>(object); \
        if(!parent) \
        { \
            qCritical()<<object<<parent; \
            qFatal(#TYPE " must be attached to a " #PARENT_TYPE); \
            return nullptr; \
        } \
        TYPE* helper = new TYPE(parent); \
        QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership); \
        return helper; \
    } \
    private: \

#define _Q_OBJECT_CHILD_ATTACHED_IMPL(TYPE, PARENT_TYPE) \
    public: \
    static TYPE* wrap(QObject* object) \
    { \
        PARENT_TYPE* parent = qobject_cast<PARENT_TYPE*>(object); \
        if (!parent) \
        { \
            qCritical()<<object<<parent; \
            qFatal(#TYPE " must be attached to a " #PARENT_TYPE); \
            return nullptr; \
        } \
        TYPE* helper = parent->findChild<TYPE*>(QString(), Qt::FindDirectChildrenOnly); \
        if(!helper) \
        { \
            helper = new TYPE(parent); \
            QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership); \
        } \
        return helper; \
    } \
    static TYPE* qmlAttachedProperties(QObject* object) \
    { \
        return wrap(object); \
    } \
    private: \

//──────────────────────────────────────────────────────────────────────
// Public: use these instead
//──────────────────────────────────────────────────────────────────────

#define Q_OBJECT_ATTACHED(TYPE, PARENT_TYPE)                    _Q_OBJECT_ATTACHED_IMPL(TYPE, PARENT_TYPE)
#define Q_OBJECT_CHILD_ATTACHED(TYPE, PARENT_TYPE)              _Q_OBJECT_CHILD_ATTACHED_IMPL(TYPE, PARENT_TYPE)

struct _QATTACHEDDEFS_ { Q_GADGET }; // mock object

#endif // QATTACHEDDEFS_H
