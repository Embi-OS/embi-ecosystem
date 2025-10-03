# CMakeLists.txt for Media Library

include(Common)

project(Postman)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Postman
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        Rest
    PLUGINS
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    QML_IMPORTS
        Eco.Tier2.Rest
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Postman
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
