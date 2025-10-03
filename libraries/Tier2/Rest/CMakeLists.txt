# CMakeLists.txt for Rest Library

include(Common)

project(Rest)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier2.Rest
    STATIC
    QT_LIBRARIES
        Network
        WebSockets
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QDefs
        QUtils
        QModels
    QML_DEPENDENCIES
        Eco.Tier1.Utils
        Eco.Tier1.Models
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        Rest
        LICENSE
        README.md
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RestCore
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RestModel
)
