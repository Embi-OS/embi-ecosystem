# CMakeLists.txt for Swupdate Library

include(Common)
include(TryLinkLibrary)

project(Swupdate)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Swupdate
    STATIC
    LIBRARIES
        Solid
    PLUGINS
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    QML_IMPORTS
        Eco.Tier2.Solid
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Swupdate
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_try_link_libraries(${PROJECT_NAME} swupdate SWUPDATE)
