#include "abstractmanagercontainer.h"
#include "axion_log.h"

#include "version.h"
#include "utils/datetimeutils.h"

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
    setRegistered(true);

    return true;
}

bool AbstractManagerContainer::postInit()
{
    for(AbstractManager* manager: m_managers->modelIterator<AbstractManager>())
    {
        AXIONLOG_INFO()<<qLogLineMessage(manager->managerName()+"::postInit");
        if(manager->postInit()) {
            manager->setReady(true);
        }
        else {
            AXIONLOG_CRITICAL()<<"Something went wrong with"<<manager->managerName();
        }
        AXIONLOG_INFO()<<qLogLine();
    }

    for(AbstractManager* manager: m_managers->modelIterator<AbstractManager>())
    {
        if(manager->getReady())
            manager->endInit();
    }

    endInit();

    setReady(true);

    return true;
}

bool AbstractManagerContainer::unInit()
{
    bool ret = true;

    setReady(false);

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
    if(!s_managerContainer->isNull())
    {
        AXIONLOG_FATAL()<<"MainManagerContainer can only be instanciated once";
    }

    *s_managerContainer = this;
}

bool MainManagerContainer::init()
{
    m_elapsedTimer.restart();

    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::init",'*');

    Version::Get()->setProductName(QCoreApplication::applicationName());

    connect(qApp, &QCoreApplication::aboutToQuit, this, &AbstractManagerContainer::unInit, Qt::SingleShotConnection);

    return AbstractManagerContainer::init();
}

bool MainManagerContainer::postInit()
{
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::postInit",'*');

    return AbstractManagerContainer::postInit();
}

void MainManagerContainer::endInit()
{
    AbstractManagerContainer::endInit();

    Version::Get()->dumpInfos();

    AXIONLOG_INFO()<<qLogLineMessage(QString("%1::ready after %2").arg(managerName(), DateTimeUtils::formatDuration(m_elapsedTimer.elapsed(), DurationFormatOptions::ShowMilliseconds)),'=');
}

bool MainManagerContainer::unInit()
{
    AXIONLOG_INFO()<<qLogLineMessage(managerName()+"::unInit",'*');

    bool ret = AbstractManagerContainer::unInit();

    AXIONLOG_INFO()<<qLogLine('*');

    return ret;
}

ManagerContainer::ManagerContainer()
{

}

MainManagerContainer* ManagerContainer::Get()
{
    if(!s_managerContainer->data())
    {
        AXIONLOG_FATAL("ManagerContainer has not been created yet !!!");
    }
    return s_managerContainer->data();
}

MainManagerContainer* ManagerContainer::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    MainManagerContainer* instance = Get();

    return instance;
}
