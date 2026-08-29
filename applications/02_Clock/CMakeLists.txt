include(Common)

project(02_Clock)

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
        Happy
        Fluid
        Solid
        Media
        Hass
        Postman
        Database
    QML_DEPENDENCIES
        Eco.Tier3.Axion
        Eco.Tier3.Happy
        Eco.Tier3.Fluid
        Eco.Tier3.Solid
        Eco.Tier3.Media
        Eco.Tier3.Hass
        Eco.Tier3.Postman
        Eco.Tier3.Database
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    IMG_FILES
        ${ALL_IMG_FILES}
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    OUTPUT_DIRECTORY_PREFIX
        /samples
)

embi_mark_boot2qt_deployable(TARGET A${PROJECT_NAME})
create_package(TARGET A${PROJECT_NAME})
