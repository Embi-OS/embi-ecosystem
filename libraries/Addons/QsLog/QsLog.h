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

#ifndef QSLOG_H
#define QSLOG_H

#include "QsLogLevel.h"
#include "QsLogDest.h"
#include "QsLogMessage.h"
#include <QDebug>
#include <QString>
#include <QDateTime>

#define QS_LOG_VERSION "2.0b3"
#define QS_LOG_TRACEESCAPE '\x1B'
#define QS_LOG_NOTICEESCAPE '\x1C'

namespace QsLogging
{

class Destination;
class LoggerImpl; // d pointer

typedef QVector<DestinationPtr> DestinationList;

class QSLOG_SHARED_OBJECT Logger
{
public:
    static void init(const QString& path);

    static Logger& instance();
    static void destroyInstance();

    ~Logger();

    //! Adds a log message destination. Don't add null destinations.
    void addDestination(DestinationPtr destination);
    //! Removes a log message destination. Don't remove null destinations.
    void removeDestination(DestinationPtr destination);
    //! Logging at a level < 'newLevel' will be ignored
    void setLoggingLevel(Level newLevel);
    //! The default level is INFO
    Level loggingLevel() const;
    //! Clear every destinations
    void clearEverything();

    //! Add a filter for log messages; empty pattern disables the filter
    void addMessageFilter(const QRegularExpression &regex);
    //! Clears the current filter
    void clearMessageFilter();

private:
    Logger();
    Logger(const Logger&);            // not available
    Logger& operator=(const Logger&); // not available

    void write(const Message& message);
    void enqueueWrite(const Message& message);

    void clear();
    void enqueueClear();

    LoggerImpl* d;

    friend class LogWriterRunnable;
    friend class LogClearerRunnable;
    friend class Helper;
};

} // end namespace

#endif // QSLOG_H
