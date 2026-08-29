include(Common)
include(Package/CreatePackage)

project(01_Gallery)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_application(${PROJECT_NAME}
    APP_NAME
        A${PROJECT_NAME}
    LIB_NAME
        L${PROJECT_NAME}
    LIBRARIES
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    QMLCACHEGEN_VERBOSE
        OFF
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    OUTPUT_DIRECTORY_PREFIX
        /samples
)

embi_mark_boot2qt_deployable(TARGET A${PROJECT_NAME})
create_package(TARGET A${PROJECT_NAME})
