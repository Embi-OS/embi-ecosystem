# CMakeLists.txt for Happy Library

include(Common)

project(Happy)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Happy
    STATIC
    QT_LIBRARIES
        HttpServer
        WebSockets
    LIBRARIES
        Sql
        Rest
    PLUGINS
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    QML_IMPORTS
        Eco.Tier2.Sql
        Eco.Tier2.Rest
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        Happy
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fields
)

