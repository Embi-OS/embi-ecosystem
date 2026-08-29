#ifndef NAVIGATIONATTACHED_H
#define NAVIGATIONATTACHED_H

#include <QDefs>

class NavigationAttached : public QObject
{
    Q_OBJECT
    QML_ATTACHED(NavigationAttached)
    QML_UNCREATABLE("Attached")
    QML_NAMED_ELEMENT(Navigation)
    
    Q_OBJECT_ATTACHED(NavigationAttached, QObject)

    Q_WRITABLE_VAR_PROPERTY(bool, guarded, Guarded, false)
    Q_WRITABLE_REF_PROPERTY(QString, name, Name, {})
    Q_WRITABLE_REF_PROPERTY(QString, secondaryName, SecondaryName, {})

    Q_WRITABLE_REF_PROPERTY(QString, effectiveName, EffectiveName, {})

private:
    explicit NavigationAttached(QObject* parent=nullptr);
};

#endif // NAVIGATIONATTACHED_H
