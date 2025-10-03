# CMakeLists.txt for Sql Library

include(Common)

project(Sql)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier2.Sql
    STATIC
    QT_LIBRARIES
        Sql
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QDeferred
        QDefs
        QUtils
        QModels
    QML_DEPENDENCIES
        Eco.Tier1.Utils
        Eco.Tier1.Models
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        Sql
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SqlCore
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SqlMigrator
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SqlModel
)
