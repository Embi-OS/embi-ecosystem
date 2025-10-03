#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include <QModels>
#include <QDefs>

class AbstractManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_READONLY_VAR_PROPERTY(bool, registered, Registered, false)
    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)
    Q_READONLY_VAR_PROPERTY(bool, initializing, Initializing, false)

    Q_READONLY_REF_PROPERTY(QString, status, Status, "")
    Q_PROPERTY(QString managerName READ managerName CONSTANT FINAL)

public:
    explicit AbstractManager(QObject* parent=nullptr);
    virtual QString managerName() const;

    Q_INVOKABLE virtual bool init()=0;
    Q_INVOKABLE virtual void postInit();
    Q_INVOKABLE virtual bool unInit() { return true; }
    Q_INVOKABLE virtual void reset() {}
    Q_INVOKABLE virtual bool isValid(){ return true; };

signals:
    void initDone(bool result);

public slots:
    void emitInitDone(bool result);
};

#endif // ABSTRACTMANAGER_H
