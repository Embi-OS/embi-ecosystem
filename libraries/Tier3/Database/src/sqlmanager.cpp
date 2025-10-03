#include "sqlmanager.h"
#include "dialogs/snackbarmanager.h"
#include "dialogs/dialogmanager.h"

#include <QtConcurrentRun>

SqlManager::SqlManager(QObject *parent) :
    AbstractManager(parent),
    m_dbPath(Paths::database())
{

}

bool SqlManager::init()
{
    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    persistantData->mapProperty(this,"dbType");
    persistantData->mapProperty(this,"dbName");
    persistantData->mapProperty(this,"dbPath");
    persistantData->mapProperty(this,"dbServer");
    persistantData->mapProperty(this,"dbUserName");
    persistantData->mapProperty(this,"dbPassword");
    persistantData->mapProperty(this,"dbPort");
    persistantData->mapProperty(this,"dbConnectOptions");

    if(m_preparatorModule.isEmpty() || m_preparatorName.isEmpty())
    {
        SqlDbProfile profile = createConnectionProfile();
        SqlDbManager::Get()->openConnection(profile);
        return true;
    }

    QScopedPointer<SqlDbPreparator> preparator(QQmlLoader::load<SqlDbPreparator>(m_preparatorModule, m_preparatorName));
    if(preparator.isNull())
    {
        return false;
    }

    preparator->setType(m_dbType);
    preparator->setName(m_dbName);
    preparator->setPath(m_dbPath);
    preparator->setServer(m_dbServer);
    preparator->setUserName(m_dbUserName);
    preparator->setPassword(m_dbPassword);
    preparator->setPort(m_dbPort);
    preparator->setConnectOptions(m_dbConnectOptions);

    bool result = SqlDbManager::Get()->open(preparator.data());

    return result;
}

bool SqlManager::unInit()
{
    return true; //SqlDbManager::Get()->close();
}

void SqlManager::setPreparator(const QString& module, const QString& name)
{
    setPreparatorModule(module);
    setPreparatorName(name);

    QScopedPointer<SqlDbPreparator> preparator(QQmlLoader::load<SqlDbPreparator>(m_preparatorModule, m_preparatorName));

    if(preparator.isNull())
        return;

    setDbType(preparator->getType());
    setDbName(preparator->getName());
    setDbPath(preparator->getPath());
    setDbServer(preparator->getServer());
    setDbUserName(preparator->getUserName());
    setDbPassword(preparator->getPassword());
    setDbPort(preparator->getPort());
    setDbConnectOptions(preparator->getConnectOptions());
}

SqlDbProfile SqlManager::createConnectionProfile()
{
    SqlDbProfile profile;

    switch(m_dbType) {
    case SqlDatabaseTypes::SQLite:
        profile.type = "QSQLITE";
        break;
    case SqlDatabaseTypes::MySQL:
        profile.type = "QMYSQL";
        break;
    default:
        break;
    }

    profile.name = m_dbName;
    profile.connectionName = SqlDefaultConnection;
    profile.path = m_dbPath;
    profile.hostName = m_dbServer;
    profile.userName = m_dbUserName;
    profile.password = m_dbPassword;
    profile.port = m_dbPort;
    profile.timeout = 10000;
    profile.connectOptions = m_dbConnectOptions;

    return profile;
}

void SqlManager::testConnexion(const QVariantMap& params)
{
    DialogObject* dialog = DialogManager::Get()->showBusy(tr("Connexion en cours"));

    SqlDbProfile profile;
    switch(params.value("dbType").toInt()) {
    case SqlDatabaseTypes::SQLite:
        profile.type = "QSQLITE";
        break;
    case SqlDatabaseTypes::MySQL:
        profile.type = "QMYSQL";
        break;
    default:
        break;
    }
    profile.name = params.value("dbName").toString();
    profile.path = params.value("dbPath").toString();
    profile.hostName = params.value("dbServer").toString();
    profile.userName = params.value("dbUserName").toString();
    profile.password = params.value("dbPassword").toString();
    profile.port = params.value("dbPort").toInt();

    QSqlError sqlError;
    SqlDbManager::Get()->testConnexion(profile)
    .complete([dialog](bool,const QSqlError&){ dialog->hide(); })
    .fail([](const QSqlError& error) {
        QVariantMap settings;
        settings["caption"] = error.text();
        settings["closable"] = true;
        SnackbarManager::Get()->showError(settings);
    })
    .done([](const QSqlError&) {
        SnackbarManager::Get()->showSuccess(tr("Connexion réussie"));
    });
}

void SqlManager::dropDatabase()
{
    SqlDbManager::Get()->dropDatabase()
    .fail([]() {
        QVariantMap settings;
        settings["message"] = tr("Impossible de supprimer la base de donnée");
        DialogManager::Get()->showError(settings);
    })
    .done([]() {
        AxionHelper::criticalRestart(tr("Suppression terminée"));
    });
}

void SqlManager::vacuumDatabase()
{
    QVariantMap settings;
    settings["message"] = tr("Vacuum");
    settings["infos"] = tr("Connection: ")+SqlDefaultConnection;
    DialogObject* dialog = DialogManager::Get()->showBusy(settings);

    auto future = SqlBuilder::vacuum().future();

    future.then(this, [dialog](const QSqlError& error){
        dialog->hide();
        if(error.isValid())
            DialogManager::Get()->showError(error.text());
        else
            DialogManager::Get()->showMessage(tr("Optimisation réussie"));
    });
}

void SqlManager::exportDatabase(const QString& path)
{
    const QString dbPath = m_dbPath;

    DialogObject* dialog = DialogManager::Get()->showBusy(tr("Export en cours!"));

    auto future = QtConcurrent::run([path, dbPath]() {
        QString dst=path+"/"+Paths::applicationName()+"_dbBackup/";
        return QUtils::Filesystem::copy(dbPath, dst, true);
    });

    future.then(this, [dialog](bool result){
        dialog->hide();
        if(result)
            DialogManager::Get()->showMessage(tr("Export terminé"));
        else
            DialogManager::Get()->showError(tr("Erreur lors de l'export"));
    });
}

void SqlManager::importDatabase(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(fileInfo.baseName()!=Paths::applicationName()+"_dbBackup")
    {
        DialogManager::Get()->showError(tr("Le chemin n'est pas compatible").arg(path));
        return;
    }

    const QString dbPath = m_dbPath;

    DialogObject* dialog = DialogManager::Get()->showBusy(tr("Import en cours!"));

    auto future = QtConcurrent::run([path, dbPath]() {
        return QUtils::Filesystem::copy(path, dbPath, true);
    });

    future.then(this, [dialog](bool result){
        dialog->hide();
        if(result)
            AxionHelper::criticalRestart(tr("Import terminé"));
        else
            DialogManager::Get()->showError(tr("Erreur lors de l'import"));
    });
}
