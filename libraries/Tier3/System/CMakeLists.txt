# CMakeLists.txt for System Library

include(Common)
include(TryLinkLibrary)

project(System)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.System
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QDeferred # TODO: Remove
        Solid
    PLUGINS
        Axion
        Swupdate
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    QML_IMPORTS
        Eco.Tier2.Solid
        Eco.Tier3.Swupdate
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        System
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
