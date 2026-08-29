include(Common)

project(03_Life)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_CELLS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cells/*)

embi_add_application(${PROJECT_NAME}
    APP_NAME
        A${PROJECT_NAME}
    LIB_NAME
        L${PROJECT_NAME}
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

qt_add_resources(L${PROJECT_NAME} "L${PROJECT_NAME}_cells"
    BASE "cells"
    PREFIX "/cells"
    FILES
        ${ALL_CELLS_FILES}
)

embi_mark_boot2qt_deployable(TARGET A${PROJECT_NAME})
create_package(TARGET A${PROJECT_NAME})
