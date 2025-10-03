# CMakeLists.txt for QsLog Library

cmake_minimum_required(VERSION 3.16)

project(QsLog
    VERSION 1.0.0
    LANGUAGES CXX
)

if(NOT DEFINED QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Qml REQUIRED)

set(ALL_SRC_FILES
    QsLog.cpp QsLog.h
    QsLogDest.cpp QsLogDest.h
    QsLogDestConsole.cpp QsLogDestConsole.h
    QsLogDestFile.cpp QsLogDestFile.h
    QsLogDestFunctor.cpp QsLogDestFunctor.h
    QsLogDestModel.cpp QsLogDestModel.h
    QsLogLevel.cpp QsLogLevel.h
    QsLogMessage.cpp QsLogMessage.h
    QsLogQmlTypes.h
)

qt_add_qml_module(${PROJECT_NAME}
    URI QsLog
    STATIC
    IMPORT_PATH ${CMAKE_BINARY_DIR}/qt/qml
    SOURCES
        ${ALL_SRC_FILES}
        README.md
        LICENSE
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_MESSAGELOGCONTEXT
    QT_DISABLE_DEPRECATED_UP_TO=0x060500
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
