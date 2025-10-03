# CMakeLists.txt for Media Library

include(Common)

project(Network)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Network
    STATIC
    LIBRARIES
        Solid
    PLUGINS
        Axion
    QML_DEPENDENCIES
        QtNetwork
        Eco.Tier3.Axion
    QML_IMPORTS
        Eco.Tier2.Solid
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Network
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
