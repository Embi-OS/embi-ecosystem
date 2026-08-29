include(Common)

project(00_Launcher)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_IMG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/*)

embi_add_application(${PROJECT_NAME}
    APP_NAME
        A${PROJECT_NAME}
    LIB_NAME
        L${PROJECT_NAME}
    LIBRARIES
        Axion
        Fluid
        Solid
    QML_DEPENDENCIES
        Eco.Tier3.Axion
        Eco.Tier3.Fluid
        Eco.Tier3.Solid
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    IMG_FILES
        ${ALL_IMG_FILES}
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

embi_mark_boot2qt_deployable(TARGET A${PROJECT_NAME})

