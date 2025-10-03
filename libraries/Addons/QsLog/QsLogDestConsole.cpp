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

#include "QsLogDestConsole.h"
#include <QString>
#include <QtGlobal>

#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
void QsDebugOutput::output( const QString& message )
{
   OutputDebugStringW(reinterpret_cast<const WCHAR*>(message.utf16()));
   OutputDebugStringW(L"\n");
}
void QsDebugOutput::outputRaw( const QString& message )
{
   OutputDebugStringW(reinterpret_cast<const WCHAR*>(message.utf16()));
}
#elif defined(Q_OS_WASM)
#include <cstdio>
void QsDebugOutput::output(const QString& message)
{
    fprintf(stdout, "%s \n", qPrintable(message));
    fflush(stdout);
}
void QsDebugOutput::outputRaw(const QString& message)
{
    fprintf(stdout, "%s", qPrintable(message));
    fflush(stdout);
}
#elif defined(Q_OS_UNIX)
#include <cstdio>
void QsDebugOutput::output(const QString& message)
{
    fprintf(stderr, "%s \n", qPrintable(message));
    fflush(stderr);
}
void QsDebugOutput::outputRaw(const QString& message)
{
    fprintf(stderr, "%s", qPrintable(message));
    fflush(stderr);
}
#endif

void QsLogging::DebugOutputDestination::clear()
{
    QsDebugOutput::outputRaw("\\e[1;1H\\e[2J");
    QsDebugOutput::outputRaw("\\033[3J");
}

void QsLogging::DebugOutputDestination::write(const Message& message)
{
    formatMessage(message);
    QsDebugOutput::output(message.display);
}

bool QsLogging::DebugOutputDestination::isValid()
{
    return true;
}
