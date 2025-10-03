# CMakeLists.txt for Media Library

include(Common)

project(Files)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Files
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
        Files
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
