# CMakeLists.txt for Unit Library

include(Common)

project(Unit)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier2.Unit
    STATIC
    LIBRARIES
        QDefs
        QUtils
        QModels
    QML_DEPENDENCIES
        Eco.Tier1.Utils
        Eco.Tier1.Models
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Unit
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
