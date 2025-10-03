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

#include "QsLogDest.h"
#include "QsLogDestConsole.h"
#include "QsLogDestFile.h"
#include "QsLogDestFunctor.h"
#include <QString>
#include <QElapsedTimer>
#include <QDebug>

namespace QsLogging
{

class DestinationImpl
{
public:
    DestinationImpl();

    Level level;
    bool includeTimestamp;
    bool includeLogLevel;
    bool includeLine;
    bool includeFunction;
    bool includeCategory;
    bool includeColor;
    char separator;
};

DestinationImpl::DestinationImpl()
    : level(NoticeLevel)
    , includeTimestamp(true)
    , includeLogLevel(true)
    , includeLine(true)
    , includeFunction(true)
    , includeCategory(true)
    , includeColor(true)
    , separator(';')
{

}

Destination::Destination()
    : d(new DestinationImpl)
{

}

Destination::~Destination()
{
    delete d;
    d = 0;
}

void Destination::setLoggingLevel(Level newLevel)
{
    d->level = newLevel;
}

Level Destination::loggingLevel() const
{
    return d->level;
}

void Destination::setIncludeTimestamp(bool e)
{
    d->includeTimestamp = e;
}

bool Destination::includeTimestamp() const
{
    return d->includeTimestamp;
}

void Destination::setIncludeLogLevel(bool l)
{
    d->includeLogLevel = l;
}

bool Destination::includeLogLevel() const
{
    return d->includeLogLevel;
}

void Destination::setIncludeLine(bool l)
{
    d->includeLine = l;
}

bool Destination::includeLine() const
{
    return d->includeLine;
}

void Destination::setIncludeFunction(bool f)
{
    d->includeFunction = f;
}

bool Destination::includeFunction() const
{
    return d->includeFunction;
}

void Destination::setIncludeCategory(bool c)
{
    d->includeCategory = c;
}

bool Destination::includeCategory() const
{
    return d->includeCategory;
}

void Destination::setIncludeColor(bool c)
{
    d->includeColor = c;
}

bool Destination::includeColor() const
{
    return d->includeColor;
}

void Destination::setSeparator(char s)
{
    d->separator = s;
}

char Destination::separator() const
{
    return d->separator;
}

void Destination::formatMessage(const Message &message)
{
    QStringList completeLine;

    if(includeTimestamp()) {
        completeLine.append(Message::formatTime(message.time));
    }

    if(includeLogLevel()) {
        completeLine.append(Message::formatLevel(message.level));
    }

    if(includeCategory()) {
        completeLine.append(Message::formatCategory(message.category));
    }

    if(includeLine()) {
        completeLine.append(Message::formatLine(message.line));
    }

    if(includeFunction()) {
        completeLine.append(Message::formatFunction(message.function));
    }

    completeLine.append(message.message);

    QString completeMessage = completeLine.join(QString("%1 ").arg(separator()));

    const QColor color = message.color();
    if(includeColor() && color.alpha()>0) {
        completeMessage.prepend(QsLogging::colorStart(color, message.level));
        completeMessage.append(QsLogging::colorEnd());
    }

    message.display = completeMessage;
}

void Destination::writeToLog(const Message& message)
{
    if(loggingLevel() > message.level)
        return;

    write(message);
}

//! destination factory
DestinationPtr DestinationFactory::MakeFileDestination(const QString& filePath,
    LogRotationOption rotation, const MaxSizeBytes &sizeInBytesToRotateAfter,
    const MaxOldLogCount &oldLogsToKeep)
{
    if (EnableLogRotation == rotation) {
        std::unique_ptr<SizeRotationStrategy> logRotation(new SizeRotationStrategy);
        logRotation->setMaximumSizeInBytes(sizeInBytesToRotateAfter.size);
        logRotation->setBackupCount(oldLogsToKeep.count);

        return DestinationPtr(new FileDestination(filePath, RotationStrategyPtr(logRotation.release())));
    }

    return DestinationPtr(new FileDestination(filePath, RotationStrategyPtr(new NullRotationStrategy)));
}

DestinationPtr DestinationFactory::MakeDebugOutputDestination()
{
    return DestinationPtr(new DebugOutputDestination);
}

DestinationPtr DestinationFactory::MakeFunctorDestination(QsLogging::Destination::LogFunction f, QsLogging::Destination::ClearFunction c)
{
    return DestinationPtr(new FunctorDestination(f, c));
}

DestinationPtr DestinationFactory::MakeFunctorDestination(QObject *receiver, const char *member)
{
    return DestinationPtr(new FunctorDestination(receiver, member));
}

} // end namespace
