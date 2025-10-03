// Copyright (c) 2013, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "QsLog.h"
#include "QsLogDest.h"
#ifdef QS_LOG_SEPARATE_THREAD
#include <QThreadPool>
#include <QRunnable>
#endif
#include <QMutex>
#include <QVector>
#include <QDateTime>
#include <QtGlobal>
#include <QRegularExpression>

namespace QsLogging
{

class Helper
{
public:
    explicit Helper(Level logLevel, const QMessageLogContext& ctx=QMessageLogContext()) :
        level(logLevel),
        context(ctx.file, ctx.line, ctx.function, ctx.category),
        qtDebug(&buffer)
    {}
    ~Helper();
    QDebug& stream(){ return qtDebug; }
    void write();

private:
    void writeToLog();

    Level level;
    QString buffer;
    QMessageLogContext context;
    QDebug qtDebug;
};

//! creates the complete log message and passes it to the logger
void Helper::writeToLog()
{
    Message message;
    message.message = buffer;
    message.time = QDateTime::currentDateTime();
    message.level = level;
    message.file = context.file;
    message.category = context.category;
    message.line = context.line;

    if(QString(context.file).contains(".qml"))
        message.function = context.file;
    else
        message.function = context.function;

    Logger::instance().enqueueWrite(message);
}

Helper::~Helper()
{

}

void Helper::write()
{
    try {
        writeToLog();
    }
    catch(std::exception&) {
        // you shouldn't throw exceptions from a sink
        Q_ASSERT(!"exception in logger helper destructor");
    }
}

static void qtMessageOutput(QtMsgType qtType, const QMessageLogContext& ctx, const QString& msg)
{
    int msgType = ctx.category[0]==QS_LOG_TRACEESCAPE ? -1 :
                  ctx.category[0]==QS_LOG_NOTICEESCAPE ? -2 : qtType;

    Level level = levelFromQtMsgType(msgType);

    const QMessageLogContext context(ctx.file, ctx.line, ctx.function, msgType<0 ? ctx.category+1 : ctx.category);
    Helper helper(level, context);
    helper.stream()<<qPrintable(msg);
    helper.write();
}

static Logger* sInstance = 0;

#ifdef QS_LOG_SEPARATE_THREAD
class LogWriterRunnable : public QRunnable
{
public:
    LogWriterRunnable(const Message& message);
    virtual void run();

private:
    Message m_message;
};
#endif


#ifdef QS_LOG_SEPARATE_THREAD
class LogClearerRunnable : public QRunnable
{
public:
    LogClearerRunnable();
    virtual void run();
};
#endif

class LoggerImpl
{
public:
    LoggerImpl();

#ifdef QS_LOG_SEPARATE_THREAD
    QThreadPool threadPool;
#endif
    QMutex logMutex;
    Level level;
    DestinationList destList;
    bool includeTimeStamp;
    bool includeLogLevel;
    QList<QRegularExpression> messageFilter;
};

#ifdef QS_LOG_SEPARATE_THREAD
LogWriterRunnable::LogWriterRunnable(const Message& message)
    : QRunnable()
    , m_message(message)
{
}

void LogWriterRunnable::run()
{
    Logger::instance().write(m_message);
}
#endif

#ifdef QS_LOG_SEPARATE_THREAD
LogClearerRunnable::LogClearerRunnable()
    : QRunnable()
{

}

void LogClearerRunnable::run()
{
    Logger::instance().clear();
}
#endif

LoggerImpl::LoggerImpl()
    : level(InfoLevel)
    , includeTimeStamp(true)
    , includeLogLevel(true)
{
    // assume at least file + console
    destList.reserve(2);
#ifdef QS_LOG_SEPARATE_THREAD
    threadPool.setMaxThreadCount(1);
    threadPool.setExpiryTimeout(-1);
#endif
}

Logger::Logger()
    : d(new LoggerImpl)
{
}

void Logger::init(const QString& path)
{
    QsLogging::DestinationPtr consoleDest = QsLogging::DestinationFactory::MakeDebugOutputDestination();
    consoleDest->setIncludeTimestamp(false);
    consoleDest->setLoggingLevel(QsLogging::NoticeLevel);
#ifdef Q_OS_WASM
    // consoleDest->setLoggingLevel(QsLogging::DebugLevel);
    // consoleDest->setIncludeColor(false);
#elif defined(QSLOG_FILE_ENABLED) && defined(QT_NO_DEBUG)
    if(!path.isEmpty())
    {
        QsLogging::DestinationPtr fileDest = QsLogging::DestinationFactory::MakeFileDestination(path,
                                                                                                QsLogging::EnableLogRotation,
                                                                                                QsLogging::MaxSizeBytes(1024 * 1024 * 100),  // Max 100 MB per file
                                                                                                QsLogging::MaxOldLogCount(4));               // Max 5 files
        fileDest->setLoggingLevel(QsLogging::DebugLevel);
        fileDest->setIncludeColor(false);
        QsLogging::Logger::instance().addDestination(fileDest);
    }
#endif
    QsLogging::Logger::instance().addDestination(consoleDest);
    QsLogging::Logger::instance().setLoggingLevel(QsLogging::NoticeLevel);

    qInfo()<<"Creating logger instance !!!";
    qInfo()<<"Logging to"<<qPrintable(path);
}

Logger& Logger::instance()
{
    if (!sInstance)
    {
        sInstance = new Logger;
        qInstallMessageHandler(QsLogging::qtMessageOutput);
    }

    return *sInstance;
}

void Logger::destroyInstance()
{
    if (sInstance)
    {
        qInstallMessageHandler(0);
        delete sInstance;
        sInstance = 0;
    }
}

Logger::~Logger()
{
#ifdef QS_LOG_SEPARATE_THREAD
    d->threadPool.waitForDone();
#endif
    delete d;
    d = 0;
}

void Logger::addDestination(DestinationPtr destination)
{
    Q_ASSERT(destination.data());
    d->destList.push_back(destination);
}

void Logger::removeDestination(DestinationPtr destination)
{
    Q_ASSERT(destination.data());
    d->destList.removeOne(destination);
}

void Logger::setLoggingLevel(Level newLevel)
{
    d->level = newLevel;
}

Level Logger::loggingLevel() const
{
    return d->level;
}

void Logger::clearEverything()
{
    enqueueClear();
}

void Logger::addMessageFilter(const QRegularExpression &regex)
{
    d->messageFilter.append(regex);
}

void Logger::clearMessageFilter()
{
    d->messageFilter.clear(); // empty (invalid) regex => no filtering
}

//! directs the message to the task queue or writes it directly
void Logger::enqueueWrite(const Message& message)
{
#ifdef QS_LOG_SEPARATE_THREAD
    LogWriterRunnable *r = new LogWriterRunnable(message);
    d->threadPool.start(r);
#else
    write(message);
#endif
}

//! Sends the message to all the destinations. The level for this message is passed in case
//! it's useful for processing in the destination.
void Logger::write(const Message& message)
{
    QMutexLocker lock(&d->logMutex);

    // If a filter is set and it does match, discard message
    for(const QRegularExpression& re: std::as_const(d->messageFilter)) {
        if (re.isValid() && !re.pattern().isEmpty()) {
            if (!re.match(message.message).hasMatch()) {
                return; // skip logging
            }
        }
    }

    if (d->level > message.level)
        return;

    for (DestinationList::iterator it = d->destList.begin(),
        endIt = d->destList.end();it != endIt;++it) {
        (*it)->writeToLog(message);
    }
}

void Logger::enqueueClear()
{
#ifdef QS_LOG_SEPARATE_THREAD
    LogClearerRunnable *r = new LogClearerRunnable();
    d->threadPool.start(r);
#else
    clear();
#endif
}

void Logger::clear()
{
    QMutexLocker lock(&d->logMutex);
    for (DestinationList::iterator it = d->destList.begin(),
        endIt = d->destList.end();it != endIt;++it) {
        (*it)->clear();
    }
}

} // end namespace
