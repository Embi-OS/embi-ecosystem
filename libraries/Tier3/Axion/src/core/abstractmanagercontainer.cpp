#include "abstractmanagercontainer.h"
#include "axion_log.h"

#include "version.h"

AbstractManagerContainer::AbstractManagerContainer(QObject *parent) :
    AbstractManagerContainer(&AbstractManager::staticMetaObject, parent)
{

}

AbstractManagerContainer::AbstractManagerContainer(const QMetaObject* metaObject, QObject *parent) :
    AbstractManager(parent),
    m_managers(new QObjectListModel(this, metaObject))
{
    m_managers->onInserted<AbstractManager>([](AbstractManager* object){
        object->setRegistered(true);
    });

    m_managers->onRemoved<AbstractManager>([](AbstractManager* object){
        object->setRegistered(false);
    });
}

bool AbstractManagerContainer::init()
{
    m_elapsedTimer.restart();

    AXIONLOG_INFO()<<qLogLine('*');
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::init",'*');

    Version::Get()->setProductName(QFileInfo(QCoreApplication::applicationFilePath()).fileName());

    connect(qApp, &QCoreApplication::aboutToQuit, this, &AbstractManagerContainer::unInit, Qt::SingleShotConnection);

    return true;
}

void AbstractManagerContainer::postInit()
{
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::postInit",'*');

    postInitManager(0);
}

void AbstractManagerContainer::endInit()
{
    AXIONLOG_INFO()<<qLogLineMessage(QString("%1::ready after %2 ms").arg(managerName()).arg(m_elapsedTimer.nsecsElapsed()/1000000.0),'=');
}

bool AbstractManagerContainer::unInit()
{
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::unInit",'*');

    bool ret = true;

    auto iterator = m_managers->modelIterator<AbstractManager>();
    iterator.toBack();
    while(iterator.hasPrevious())
    {
        AbstractManager* manager = iterator.previous();
        AXIONLOG_INFO()<<qLogLineMessage(manager->managerName()+"::unInit", '-');
        if(manager->unInit())
        {
            AXIONLOG_INFO()<<qPrintable(QString("%1: Manager ->").arg(managerName()))<<manager->managerName()<<"uninitialized";
            manager->setReady(false);
        }
        else
            ret = false;
        AXIONLOG_INFO()<<qLogLine('-');
    }

    AXIONLOG_INFO()<<qLogLine('*');

    setReady(false);

    return ret;
}

void AbstractManagerContainer::reset()
{
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::reset",'*');

    for(AbstractManager* manager : m_managers->modelIterator<AbstractManager>())
    {
        AXIONLOG_INFO()<<qLogLineMessage(manager->managerName()+"::reset");
        manager->reset();
        AXIONLOG_INFO()<<qLogLine();
    }

    AXIONLOG_INFO()<<qLogLine('*');
}

bool AbstractManagerContainer::isValid()
{
    bool ret=true;

    for(AbstractManager* manager : m_managers->modelIterator<AbstractManager>())
    {
        if(!manager->isValid())
            ret = false;
    }

    return ret;
}

void AbstractManagerContainer::postInitManager(int index)
{
    if(index<0 || index>=m_managers->count())
    {
        if(getReady())
            return;
        endInit();
        emitInitDone(true);
        return;
    }

    AbstractManager* manager = m_managers->at<AbstractManager>(index);
    AXIONLOG_INFO()<<qLogLineMessage(manager->managerName()+"::postInit");
    manager->postInit();

    auto queuePostInitManager = [this, index, manager](){
        bool result = manager->getReady();
        if(!result) {
            AXIONLOG_CRITICAL()<<"Something went wrong with"<<manager->managerName();
        }
        AXIONLOG_INFO()<<qLogLine();
        QMetaObject::invokeMethod(this, &AbstractManagerContainer::postInitManager, Qt::QueuedConnection, index+1);
    };

    if(!manager->getInitializing())
    {
        queuePostInitManager();
        return;
    }

    connect(manager, &AbstractManager::initDone, this, std::move(queuePostInitManager), Qt::SingleShotConnection);
}

bool AbstractManagerContainer::registerManager(AbstractManager* manager)
{
    if(manager==nullptr)
        return false;

    if (m_managers->contains(manager))
    {
        AXIONLOG_WARNING()<<qPrintable(QString("%1:").arg(managerName()))<<manager->managerName()<<"already registered!";
        return false;
    }

    manager->setParent(this);
    QQmlEngine::setObjectOwnership(manager, QQmlEngine::CppOwnership);

    if (manager->getReady())
    {
        AXIONLOG_WARNING()<<qPrintable(QString("%1:").arg(managerName()))<<manager->managerName()<<"already initialized!";
        m_managers->append(manager);
        return true;
    }

    AXIONLOG_INFO()<<qLogLineMessage(manager->managerName()+"::init",'-');
    manager->setInitializing(true);
    if (!manager->init())
    {
        AXIONLOG_CRITICAL()<<"Failed to init manager:"<<manager->managerName();
        return false;
    }
    AXIONLOG_INFO()<<qLogLine('-');

    m_managers->append(manager);

    return true;
}

Q_GLOBAL_STATIC(QPointer<MainManagerContainer>, s_managerContainer)
MainManagerContainer::MainManagerContainer(QObject *parent) :
    AbstractManagerContainer(parent)
{
    Q_ASSERT_X(s_managerContainer->isNull(), "MainManagerContainer", "MainManagerContainer can only be instanciated once");

    if(!s_managerContainer->isNull())
    {
        AXIONLOG_FATAL()<<"MainManagerContainer can only be instanciated once";
    }

    *s_managerContainer = this;
}

void MainManagerContainer::endInit()
{
    AbstractManagerContainer::endInit();

    Version::Get()->dumpInfos();
}

ManagerContainer::ManagerContainer()
{

}

MainManagerContainer* ManagerContainer::Get()
{
    return s_managerContainer->data();
}

MainManagerContainer* ManagerContainer::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    MainManagerContainer* instance = Get();

    return instance;
}
