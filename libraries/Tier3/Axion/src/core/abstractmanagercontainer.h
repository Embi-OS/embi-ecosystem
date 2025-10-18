#ifndef ABSTRACTMANAGERCONTAINER_H
#define ABSTRACTMANAGERCONTAINER_H

#include "abstractmanager.h"
#include <QModels>
class AbstractManagerContainer : public AbstractManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_COMPOSITION_PROPERTY(QObjectListModel, managers, nullptr)

public:
    explicit AbstractManagerContainer(QObject *parent = nullptr);
    explicit AbstractManagerContainer(const QMetaObject* metaObject, QObject *parent = nullptr);

    bool init() override;
    void postInit() override;
    bool unInit() final override;
    void reset() final override;
    bool isValid() final override;

    bool registerManager(AbstractManager* manager);

protected slots:
    void postInitManager(int index);

protected:
    virtual void endInit();

private:
    QElapsedTimer m_elapsedTimer;
};

class MainManagerContainer: public AbstractManagerContainer
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit MainManagerContainer(QObject *parent = nullptr);

protected:
    void endInit() override;
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
