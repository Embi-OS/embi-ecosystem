# CMakeLists.txt for Fluid Library

include(Common)

project(Fluid)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Fluid
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
    PLUGINS
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Fluid
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/display
        ${CMAKE_CURRENT_SOURCE_DIR}/src/maintenance
        ${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network
)
