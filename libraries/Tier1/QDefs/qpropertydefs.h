#ifndef QPROPERTYDEFS_H
#define QPROPERTYDEFS_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlProperty>

//──────────────────────────────────────────────────────────────────────
// Private: for declaration purpose
//──────────────────────────────────────────────────────────────────────

#define _Q_PROPERTY_MEMBER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    VAR_TYPE m_##name=__VA_ARGS__;

#define _Q_PROPERTY_PTR_MEMBER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    VAR_TYPE m_##name=__VA_ARGS__; \

#define _Q_PROPERTY_PTR_DEFAULT_MEMBER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    VAR_TYPE m_default##Name=__VA_ARGS__;

#define _Q_PROPERTY_GETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    RET_TYPE get##Name (void) const { \
        return m_##name; \
    }

#define _Q_PROPERTY_PTR_GETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    RET_TYPE get##Name (void) const { \
        if(m_##name==nullptr) \
            return m_default##Name; \
        return m_##name; \
    }

#define _Q_ABSTRACT_GETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    virtual RET_TYPE get##Name (void) const = 0;

#define _Q_PROPERTY_SETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    bool set##Name (ARG_TYPE name) { \
        if (m_##name == name) return false; \
        m_##name = name; \
        return true; \
    }

#define _Q_PROPERTY_RESET_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    bool reset##Name (void) { \
        return set##Name (__VA_ARGS__); \
    }

#define _Q_PROPERTY_NOTIFIABLE_SETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    bool set##Name (ARG_TYPE name) { \
        if (m_##name == name) return false; \
        m_##name = name; \
        emit name##Changed (m_##name); \
        return true; \
    }

#define _Q_PROPERTY_FULL_SETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    bool set##Name (ARG_TYPE name) { \
        if (m_##name == name) return false; \
        emit name##AboutToChange (m_##name, name); \
        m_##name = name; \
        emit name##Changed (m_##name); \
        return true; \
    }

#define _Q_PROPERTY_FUZ_SETTER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    bool set##Name (ARG_TYPE name) { \
        if (qFuzzyCompare(m_##name, name)) return false; \
        m_##name = (VAR_TYPE)name; \
        emit name##Changed (m_##name); \
        return true; \
    }

#define _Q_PROPERTY_SIGNAL_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    void name##Changed (SIG_TYPE name);

#define _Q_PROPERTY_NOTIFIER_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    void name##AboutToChange (SIG_TYPE old##Name, SIG_TYPE new##Name); \

#define _Q_CALLABLE_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_NOTIFIABLE_SETTER_IMPL     (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_RESET_IMPL                 (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_WRITABLE_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_NOTIFIABLE_SETTER_IMPL     (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL                 (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_READONLY_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL              (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_NOTIFIABLE_SETTER_IMPL   (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL               (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL              (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL              (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_CALLABLE_PTR_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_PTR_GETTER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_FULL_SETTER_IMPL               (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_RESET_IMPL                     (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_NOTIFIER_IMPL                  (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL                    (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_DEFAULT_MEMBER_IMPL        (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_MEMBER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_WRITABLE_PTR_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_PTR_GETTER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_FULL_SETTER_IMPL               (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL                     (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_NOTIFIER_IMPL                  (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL                    (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_DEFAULT_MEMBER_IMPL        (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_MEMBER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_READONLY_PTR_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_PTR_GETTER_IMPL              (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_FULL_SETTER_IMPL             (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL                   (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_NOTIFIER_IMPL                (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL                  (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_DEFAULT_MEMBER_IMPL      (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_PTR_MEMBER_IMPL              (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_CALLABLE_FUZ_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_FUZ_SETTER_IMPL   (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public Q_SLOTS: _Q_PROPERTY_RESET_IMPL        (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_WRITABLE_FUZ_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name WRITE set##Name RESET reset##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_FUZ_SETTER_IMPL   (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL        (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_READONLY_FUZ_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name NOTIFY name##Changed PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_FUZ_SETTER_IMPL   (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    public:         _Q_PROPERTY_RESET_IMPL        (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    Q_SIGNALS:      _Q_PROPERTY_SIGNAL_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL       (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_CONSTANT_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name PROP_PARAMS) \
    public:         _Q_PROPERTY_GETTER_IMPL (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    protected:      _Q_PROPERTY_MEMBER_IMPL (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_ABSTRACT_PROPERTY_IMPL(name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (PROP_TYPE name READ get##Name PROP_PARAMS) \
    public:         _Q_ABSTRACT_GETTER_IMPL (name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE, SIG_TYPE, RET_TYPE, PROP_PARAMS, __VA_ARGS__) \
    private:

#define _Q_COMPOSITION_PROPERTY_IMPL(name, TYPE, PROP_PARAMS, ...) \
    private: \
    Q_PROPERTY (TYPE* name READ name PROP_PARAMS) \
    public:    TYPE* name (void) const { return m_##name; } \
    protected:      _Q_PROPERTY_MEMBER_IMPL (name, Name, TYPE*, TYPE*, TYPE*, TYPE*, TYPE*, PROP_PARAMS, __VA_ARGS__) \
    protected: friend TYPE;

#define _Q_MEMBER_PROPERTY_IMPL(name, TYPE, PROP_PARAMS, ...) \
private: \
    Q_PROPERTY (TYPE name MEMBER name PROP_PARAMS) \
    public: TYPE name=__VA_ARGS__; \
    private:

//──────────────────────────────────────────────────────────────────────
// Public: use these instead
//──────────────────────────────────────────────────────────────────────
                                                                                    // name, Name, VAR_TYPE, PROP_TYPE, ARG_TYPE,     SIG_TYPE,     RET_TYPE,     PROP_PARAMS,      __VA_ARGS__
#define Q_REQUIRED_VAR_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL REQUIRED,   __VA_ARGS__)
#define Q_CALLABLE_VAR_PROPERTY(TYPE, name, Name, ...)  _Q_CALLABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_WRITABLE_VAR_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_READONLY_VAR_PROPERTY(TYPE, name, Name, ...)  _Q_READONLY_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_CONSTANT_VAR_PROPERTY(TYPE, name, Name, ...)  _Q_CONSTANT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         CONSTANT FINAL,   __VA_ARGS__)
#define Q_ABSTRACT_VAR_PROPERTY(TYPE, name, Name)       _Q_ABSTRACT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         CONSTANT FINAL,   {})

#define Q_REQUIRED_FUZ_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_FUZ_PROPERTY_IMPL (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL REQUIRED,   __VA_ARGS__)
#define Q_CALLABLE_FUZ_PROPERTY(TYPE, name, Name, ...)  _Q_CALLABLE_FUZ_PROPERTY_IMPL (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_WRITABLE_FUZ_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_FUZ_PROPERTY_IMPL (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_READONLY_FUZ_PROPERTY(TYPE, name, Name, ...)  _Q_READONLY_FUZ_PROPERTY_IMPL (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         FINAL,            __VA_ARGS__)
#define Q_CONSTANT_FUZ_PROPERTY(TYPE, name, Name, ...)  _Q_CONSTANT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         CONSTANT FINAL,   __VA_ARGS__)
#define Q_ABSTRACT_FUZ_PROPERTY(TYPE, name, Name)       _Q_ABSTRACT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      TYPE,         TYPE,         TYPE,         CONSTANT FINAL,   {})

#define Q_REQUIRED_REF_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  FINAL REQUIRED,   __VA_ARGS__)
#define Q_CALLABLE_REF_PROPERTY(TYPE, name, Name, ...)  _Q_CALLABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  FINAL,            __VA_ARGS__)
#define Q_WRITABLE_REF_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  FINAL,            __VA_ARGS__)
#define Q_READONLY_REF_PROPERTY(TYPE, name, Name, ...)  _Q_READONLY_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  FINAL,            __VA_ARGS__)
#define Q_CONSTANT_REF_PROPERTY(TYPE, name, Name, ...)  _Q_CONSTANT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  CONSTANT FINAL,   __VA_ARGS__)
#define Q_ABSTRACT_REF_PROPERTY(TYPE, name, Name)       _Q_ABSTRACT_PROPERTY_IMPL     (name, Name, TYPE,     TYPE,      const TYPE&,  const TYPE&,  const TYPE&,  CONSTANT FINAL,   {})

#define Q_REQUIRED_PTR_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PTR_PROPERTY_IMPL (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        FINAL REQUIRED,   __VA_ARGS__)
#define Q_CALLABLE_PTR_PROPERTY(TYPE, name, Name, ...)  _Q_CALLABLE_PTR_PROPERTY_IMPL (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        FINAL,            __VA_ARGS__)
#define Q_WRITABLE_PTR_PROPERTY(TYPE, name, Name, ...)  _Q_WRITABLE_PTR_PROPERTY_IMPL (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        FINAL,            __VA_ARGS__)
#define Q_READONLY_PTR_PROPERTY(TYPE, name, Name, ...)  _Q_READONLY_PTR_PROPERTY_IMPL (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        FINAL,            __VA_ARGS__)
#define Q_CONSTANT_PTR_PROPERTY(TYPE, name, Name, ...)  _Q_CONSTANT_PROPERTY_IMPL     (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        CONSTANT FINAL,   __VA_ARGS__)
#define Q_ABSTRACT_PTR_PROPERTY(TYPE, name, Name)       _Q_ABSTRACT_PROPERTY_IMPL     (name, Name, TYPE*,    TYPE*,     TYPE*,        TYPE*,        TYPE*,        CONSTANT FINAL,   {})

#define Q_COMPOSITION_PROPERTY(TYPE, name, ...)         _Q_COMPOSITION_PROPERTY_IMPL  (name, TYPE, CONSTANT FINAL, __VA_ARGS__)

#define Q_MEMBER_PROPERTY(TYPE, name, ...)              _Q_MEMBER_PROPERTY_IMPL(name, TYPE, FINAL, __VA_ARGS__)

#define Q_DEFAULT_PROPERTY(NAME) \
    private: Q_CLASSINFO ("DefaultProperty", #NAME)

#define Q_PARENT_PROPERTY(NAME) \
    private: Q_CLASSINFO ("ParentProperty", #NAME)

#endif // QPROPERTYDEFS_H
