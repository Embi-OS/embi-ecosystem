#include "log.h"
#include "paths.h"
#include "axion_log.h"

#include "QsLog.h"

#include <QUtils>

Log::Log(QObject *parent) :
    QObject(parent)
{

}

bool Log::m_ready = false;
void Log::init()
{
    if(m_ready)
        return;
    m_ready = true;

    QsLogging::Logger::init(Paths::log("Log.log"));

    // TODO: QTBUG-139113
    static QRegularExpression regExp("^(?!QObject::disconnect: wildcard call disconnects from destroyed signal of).*");
    QsLogging::Logger::instance().addMessageFilter(regExp);
}

void Log::unInit()
{
    m_ready = false;

    AXIONLOG_INFO()<<"Destroying logger instance !!!";
    QsLogging::Logger::destroyInstance();
}

bool Log::clear()
{
    AXIONLOG_INFO()<<"Clearing logs !!!";

    bool result = QUtils::Filesystem::clear(Paths::log());
    QsLogging::Logger::instance().clearEverything();
    return result;
}

bool Log::copy(const QString& path)
{
    AXIONLOG_INFO()<<"Exporting logs !!!";

    QString dst=path+"/"+Paths::applicationFileName()+"_logs/";
    return QUtils::Filesystem::copy(Paths::log(), dst, true);
}

Q_GLOBAL_STATIC(QElapsedTimer, g_elapsedTimer)
void Log::startElapsed() const
{
    g_elapsedTimer->start();
}
double Log::elapsed() const
{
    return g_elapsedTimer->nsecsElapsed()/1000000.0;
}

#define QMLLOG_TRACE QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1B""QML").debug
#define QMLLOG_DEBUG QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"QML").debug
#define QMLLOG_INFO QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"QML").info
#define QMLLOG_WARNING QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"QML").warning
#define QMLLOG_CRITICAL QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"QML").critical
#define QMLLOG_FATAL QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"QML").fatal

void Log::trace(const QString& message) const
{
    QMLLOG_TRACE().noquote()<<message;
}

void Log::debug(const QString& message) const
{
    QMLLOG_DEBUG().noquote()<<message;
}

void Log::info(const QString& message) const
{
    QMLLOG_INFO().noquote()<<message;
}

void Log::warning(const QString& message) const
{
    QMLLOG_WARNING().noquote()<<message;
}

void Log::critical(const QString& message) const
{
    QMLLOG_CRITICAL().noquote()<<message;
}

void Log::fatal(const QString& message) const
{
    QMLLOG_FATAL().noquote()<<message;
}

