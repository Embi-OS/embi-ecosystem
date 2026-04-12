#ifndef ABSTRACTMANAGERCONTAINER_H
#define ABSTRACTMANAGERCONTAINER_H

#include "abstractmanager.h"
#include <QModels>
class AbstractManagerContainer : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_PTR_PROPERTY(QObjectListModel, managers)

public:
    explicit AbstractManagerContainer(QObject *parent = nullptr);
    explicit AbstractManagerContainer(const QMetaObject* metaObject, QObject *parent = nullptr);

    bool init() override;
    bool postInit() override;
    bool unInit() override;
    void reset() final override;
    bool isValid() final override;

    bool registerManager(AbstractManager* manager);
};

class MainManagerContainer: public AbstractManagerContainer
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit MainManagerContainer(QObject *parent = nullptr);

    bool init() override;
    bool postInit() final override;
    void endInit() override;
    bool unInit() final override;

private:
    QElapsedTimer m_elapsedTimer;
};

struct ManagerContainer
{
    Q_GADGET
    QML_FOREIGN(MainManagerContainer)
    QML_SINGLETON
    QML_NAMED_ELEMENT(ManagerContainer)

private:
    ManagerContainer();

public:
    static MainManagerContainer* Get();
    static MainManagerContainer* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
};

#endif // ABSTRACTMANAGERCONTAINER_H
