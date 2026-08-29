#ifndef QENUMDEFS_H
#define QENUMDEFS_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QMetaEnum>

namespace QEnumDisplay {
inline QString fallback(const int)
{
    return QString();
}
}

#define Q_ENUM_CLASS(NAMESPACE_NAME, ENUM_NAME, ...) \
    Q_ENUM_CLASS_I18N(NAMESPACE_NAME, ENUM_NAME, QEnumDisplay::fallback, __VA_ARGS__)

#define Q_ENUM_CLASS_I18N(NAMESPACE_NAME, ENUM_NAME, DISPLAY_TEXT_FUNCTION, ...) \
    namespace NAMESPACE_NAME { \
        Q_NAMESPACE \
        enum Enum: int {__VA_ARGS__} ; \
        inline bool isValid (const int value) { \
            return bool (staticMetaObject.enumerator (0).valueToKey (value) != nullptr); \
        } \
        inline QByteArray asByteArray (const int value) { \
            if (!isValid (value)) \
                return QByteArray ("Invalid enum value"); \
            return QByteArray (staticMetaObject.enumerator (0).valueToKey (value)); \
        } \
        inline QString asString (const int value) { \
            return QString::fromLatin1 (asByteArray (value)); \
        } \
        inline QString displayText (const int value) { \
            const QString display = DISPLAY_TEXT_FUNCTION (value); \
            return !display.isEmpty() ? display : asString (value); \
        } \
        inline NAMESPACE_NAME::Enum fromByteArray (const QByteArray & str, bool * ok = nullptr) { \
            return NAMESPACE_NAME::Enum (staticMetaObject.enumerator (0).keyToValue (str.constData (), ok)); \
        } \
        inline NAMESPACE_NAME::Enum fromString (const QString & str, bool * ok = nullptr) { \
            return fromByteArray (str.toLatin1 (), ok); \
        } \
        inline NAMESPACE_NAME::Enum fromInt (const int value) { \
            return NAMESPACE_NAME::Enum (value); \
        } \
        Q_ENUM_NS (Enum) \
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
        Q_INVOKABLE QString displayText(const int value) { \
            if(auto engine = qmlEngine(this)) \
                engine->markCurrentFunctionAsTranslationBinding(); \
            return NAMESPACE_NAME::displayText(value); \
        } \
        Q_INVOKABLE NAMESPACE_NAME::Enum fromByteArray(const QByteArray & str) { return NAMESPACE_NAME::fromByteArray(str); } \
        Q_INVOKABLE NAMESPACE_NAME::Enum fromString(const QString & str) { return NAMESPACE_NAME::fromString(str); } \
        Q_INVOKABLE NAMESPACE_NAME::Enum fromInt(const int value) { return NAMESPACE_NAME::fromInt(value); } \
    }; \
    QML_DECLARE_TYPE(NAMESPACE_NAME::Enum) \
    Q_DECLARE_METATYPE(NAMESPACE_NAME::Enum) \
    typedef NAMESPACE_NAME::Enum ENUM_NAME;

#endif // QENUMDEFS_H
