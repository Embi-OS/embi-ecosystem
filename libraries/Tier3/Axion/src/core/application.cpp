#include "application.h"
#include "axion_log.h"

#include "version.h"
#include "log.h"
#include "paths.h"
#include "axion_helpertypes.h"

#ifdef Q_OS_WASM
#include <emscripten/emscripten.h>
#endif

static QApplication *createQtApplication(int &argc, char **argv, const QString &applicationName)
{
    QGuiApplication::setOrganizationName(PROJECT_COMPANY);
    QGuiApplication::setOrganizationDomain(PROJECT_DOMAIN);
    QGuiApplication::setApplicationName(applicationName);
    QGuiApplication::setApplicationDisplayName(PROJECT_COPYRIGHT);
    QGuiApplication::setApplicationVersion(Version::Get()->getVersion());

    QQuickStyle::setStyle(QStringLiteral("Material"));

#ifndef Q_OS_WASM
    qputenv("QT_MEDIA_BACKEND", QByteArray("gstreamer"));
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_STYLE", QByteArray("axion"));
#endif

    // qputenv("QSG_VISUALIZE", QByteArray("overdraw"));
    // qputenv("QT_DEBUG_PLUGINS", QByteArray("1"));
    // qputenv("QML_IMPORT_TRACE", QByteArray("1"));
    // qputenv("QML_DISABLE_DISK_CACHE", QByteArray("1"));

    QApplication *app = new QApplication(argc, argv);

    return app;
}

Application::Application(int &argc, char **argv, const QString &applicationName) :
    m_application(createQtApplication(argc, argv, applicationName)),
    m_engine(new QQmlApplicationEngine)
{
    AXIONLOG_INFO()<<"Constructing Application...";

    QObject::connect(m_engine.data(), &QQmlApplicationEngine::objectCreationFailed, m_application.data(), []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    QObject::connect(m_engine.data(), &QQmlApplicationEngine::quit, m_application.data(), &QCoreApplication::quit, Qt::QueuedConnection);
    QObject::connect(m_engine.data(), &QQmlApplicationEngine::exit, m_application.data(), &QCoreApplication::exit, Qt::QueuedConnection);

    Log::init();
    Paths::init();

    AxionHelper::parseArguments();
    AxionHelper::registerComponents(*m_engine);

    installTranslators();
}

Application::~Application()
{
    m_engine.reset();
    m_application.reset();

    Log::unInit();
}

int Application::run(QAnyStringView uri, QAnyStringView typeName)
{
    AXIONLOG_INFO()<<"Loading QML...";

    Q_DEBUG_TIME(m_engine->loadFromModule(uri, typeName);)

    if (m_engine->rootObjects().isEmpty()) {
        AXIONLOG_CRITICAL()<<"Failed to load QML !";
    }

    AXIONLOG_INFO()<<"Launching Application...";

#ifdef Q_OS_WASM
    QObject::connect(m_application.data(), &QCoreApplication::aboutToQuit, m_application.data(), [this](){
        ::emscripten_async_call([](void *application) {
            delete static_cast<Application*>(application);
            ::emscripten_force_exit(0);
        }, this, 50); // Add a timeout to let potential late threads exit
    });
    // Still use exec here so that we can call Qt.quit from QML or qApp.quit from cpp
    // We can then rely on &QCoreApplication::aboutToQuit to then exit WASM
    return m_application->exec();
#else
    QScopedPointer<Application> that(this);
    return m_application->exec();
#endif
}

void Application::installTranslators()
{
    const QVariantMap args = AxionHelper::Get()->getArguments();
    if(args.contains("lang"))
    {
        const QString lang = args.value("lang").toString();
        QTranslatorLoader* translator = new QTranslatorLoader(m_application.data());
        translator->componentComplete();
        translator->setLanguage(QLocale(lang).name().split("_").at(0));
    }
}
