include(Common)

project(03_Life)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_CELLS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cells/*)

set(APP_NAME A${PROJECT_NAME})
set(LIB_NAME L${PROJECT_NAME})

embi_add_application(${PROJECT_NAME}
    APP_NAME
        ${APP_NAME}
    LIB_NAME
        ${LIB_NAME}
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QDefs
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    OUTPUT_DIRECTORY_PREFIX
        /samples
)

qt_add_resources(${LIB_NAME} "${LIB_NAME}_cells"
    BASE "cells"
    PREFIX "/cells"
    FILES
        ${ALL_CELLS_FILES}
)
