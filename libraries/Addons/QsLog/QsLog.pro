
TEMPLATE = lib

! include($$PWD/../../../_Common.pri) {
    error( "Couldn't find the _Common.pri file!" )
}

INCLUDEPATH += $$PWD

#DEFINES += QS_LOG_SEPARATE_THREAD # messages are queued and written from a separate thread

SOURCES += \
    $$PWD/QsLogDest.cpp \
    $$PWD/QsLog.cpp \
    $$PWD/QsLogDestConsole.cpp \
    $$PWD/QsLogDestFile.cpp \
    $$PWD/QsLogDestFunctor.cpp \
    $$PWD/QsLogLevel.cpp \
    $$PWD/QsLogDestModel.cpp \
    $$PWD/QsLogMessage.cpp

HEADERS += \
    $$PWD/QsLogDest.h \
    $$PWD/QsLog.h \
    $$PWD/QsLogDestConsole.h \
    $$PWD/QsLogLevel.h \
    $$PWD/QsLogDestFile.h \
    $$PWD/QsLogDestFunctor.h \
    $$PWD/QsLogMessage.h \
    $$PWD/QsLogDestModel.h

OTHER_FILES += \
    $$PWD/README.md \
    $$PWD/LICENSE
