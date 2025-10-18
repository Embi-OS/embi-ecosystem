include(Common)
include(CreateDebPackage)

project(01_Gallery)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

set(APP_NAME A${PROJECT_NAME})
set(LIB_NAME L${PROJECT_NAME})

embi_add_application(${PROJECT_NAME}
    APP_NAME
        ${APP_NAME}
    LIB_NAME
        ${LIB_NAME}
    LIBRARIES
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    OUTPUT_DIRECTORY_PREFIX
        /samples
)

create_deb_package(${APP_NAME})
