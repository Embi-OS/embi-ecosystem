include(Common)

project(00_Launcher)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_IMG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/*)

set(APP_NAME A${PROJECT_NAME})
set(LIB_NAME L${PROJECT_NAME})

embi_add_application(${PROJECT_NAME}
    APP_NAME
        ${APP_NAME}
    LIB_NAME
        ${LIB_NAME}
    LIBRARIES
        Axion
        Files
        Network
        System
    QML_DEPENDENCIES
        Eco.Tier3.Axion
        Eco.Tier3.Files
        Eco.Tier3.Network
        Eco.Tier3.System
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    IMG_FILES
        ${ALL_IMG_FILES}
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
