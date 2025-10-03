#ifndef QFLAGDEFS_H
#define QFLAGDEFS_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QMetaEnum>

#define Q_FLAG_CLASS(NAMESPACE_NAME, FLAG_NAME, ENUM_NAME, ...) \
    namespace NAMESPACE_NAME { \
        Q_NAMESPACE \
        enum Option: int { __VA_ARGS__ }; \
        Q_DECLARE_FLAGS(Flag, Option) \
        Q_DECLARE_OPERATORS_FOR_FLAGS (NAMESPACE_NAME::Flag); \
        inline bool isValid (const int value) { \
            return bool (staticMetaObject.enumerator (0).valueToKey (value) != nullptr); \
        } \
        inline QByteArray asByteArray (const int value) { \
            return QByteArray (staticMetaObject.enumerator (0).valueToKeys (value)); \
        } \
        inline QString asString (const int value) { \
            return QString::fromLatin1 (asByteArray (value)); \
        } \
        inline Option fromByteArray (const QByteArray & str, bool * ok = nullptr) { \
            return NAMESPACE_NAME::Option (staticMetaObject.enumerator (0).keyToValue (str.constData (), ok)); \
        } \
        inline Option fromString (const QString & str, bool * ok = nullptr) { \
            return fromByteArray (str.toLatin1 (), ok); \
        } \
        inline NAMESPACE_NAME::Option fromInt (const int value) { \
                return NAMESPACE_NAME::Option (value); \
        } \
        inline QString dump(NAMESPACE_NAME::Flag options) { \
            QStringList strList; \
            QMetaEnum me = NAMESPACE_NAME::staticMetaObject.enumerator(0); \
            for (int i=0; i<me.keyCount(); ++i) { \
                if (options.testFlag((NAMESPACE_NAME::Option)me.value(i))) { \
                    strList.append(me.key(i)); \
                } \
            } \
            return strList.join(" | "); \
        } \
        Q_FLAG_NS(Flag) \
    }; \
    class NAMESPACE_NAME##Extended : public QObject { \
        Q_OBJECT \
        QML_NAMED_ELEMENT(NAMESPACE_NAME) \
        QML_EXTENDED_NAMESPACE(NAMESPACE_NAME) \
        QML_SINGLETON \
        public: \
        using QObject::QObject; \
        Q_INVOKABLE bool isValid(const int value) { return NAMESPACE_NAME::isValid(value); } \
        Q_INVOKABLE QByteArray asByteArray(const int value) { return NAMESPACE_NAME::asByteArray(value); } \
        Q_INVOKABLE QString asString(const int value) { return NAMESPACE_NAME::asString(value); } \
        Q_INVOKABLE NAMESPACE_NAME::Option fromByteArray(const QByteArray & str) { return NAMESPACE_NAME::fromByteArray(str); } \
        Q_INVOKABLE NAMESPACE_NAME::Option fromString(const QString & str) { return NAMESPACE_NAME::fromString(str); } \
        Q_INVOKABLE NAMESPACE_NAME::Option fromInt(const int value) { return NAMESPACE_NAME::fromInt(value); } \
        Q_INVOKABLE QString dump(NAMESPACE_NAME::Flag options) { return NAMESPACE_NAME::dump(options); } \
    }; \
    QML_DECLARE_TYPE(NAMESPACE_NAME::Option) \
    Q_DECLARE_METATYPE(NAMESPACE_NAME::Option) \
    typedef NAMESPACE_NAME::Flag FLAG_NAME; \
    typedef NAMESPACE_NAME::Option ENUM_NAME;

#endif // QFLAGDEFS_H
