
TEMPLATE = lib

! include($$PWD/../../../_Common.pri) {
    error( "Couldn't find the _Common.pri file!" )
}

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qlambdathreadworkerdata.h \
    $$PWD/qlambdathreadworker.h \
    $$PWD/qdeferred.hpp \
    $$PWD/qdeferreddata.hpp \
    $$PWD/qdynamicevents.hpp \
    $$PWD/qdynamiceventsdata.hpp \
    $$PWD/qeventer.h

SOURCES += \
    $$PWD/qlambdathreadworkerdata.cpp \
    $$PWD/qlambdathreadworker.cpp \
    $$PWD/qdeferreddata.cpp \
    $$PWD/qdynamiceventsdata.cpp \
    $$PWD/qeventer.cpp

DISTFILES += \
    $$PWD/QDeferred \
    $$PWD/QEventer \
    $$PWD/README.md \
    $$PWD/LICENSE
