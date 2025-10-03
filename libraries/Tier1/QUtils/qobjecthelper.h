#ifndef QOBJECTHELPER_H
#define QOBJECTHELPER_H

#include <QObject>
#include <QQmlEngine>

class QObjectHelper : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ObjectHelper)
    QML_UNCREATABLE("Attached")
    QML_ATTACHED(QObjectHelper)

    // ──────── CONSTRUCTOR ──────────
private:
    explicit QObjectHelper(QObject* object);

public:
    static QObjectHelper* wrap(const QObject* object);
    static QObjectHelper* qmlAttachedProperties(QObject* object);

    // ──────── PUBLIC API ──────────
public:
    Q_INVOKABLE bool set(const QVariantMap& data);
    Q_INVOKABLE bool setProperty(const QString& property, const QVariant& value);
    Q_INVOKABLE QVariantMap get(const QStringList& properties=QStringList()) const;
    Q_INVOKABLE QVariant getProperty(const QString& property) const;
    Q_INVOKABLE bool reset(const QStringList& properties=QStringList());
    Q_INVOKABLE bool resetProperty(const QString& property);

    // ──────── PUBLIC STATIC API ──────────
public:
    static bool set(QObject* object, const QVariantMap& data);
    static bool setProperty(QObject* object, const QString& property, const QVariant& value);
    static QVariantMap get(const QObject* object, const QStringList& properties=QStringList());
    static QVariant getProperty(const QObject* object, const QString& property);
    static bool reset(QObject* object, const QStringList& properties=QStringList());
    static bool resetProperty(QObject* object, const QString& property);

    static QVariant readProperty(const QObject* object, const QMetaProperty &property);
    static bool writeProperty(QObject *object, const QMetaProperty &property, const QVariant &value);
    static bool clearProperty(QObject *object, const QMetaProperty &property);

private:
    QObject* m_object;
};

#endif // QOBJECTHELPER_H
