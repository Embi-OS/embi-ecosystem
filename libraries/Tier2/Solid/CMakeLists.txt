# CMakeLists.txt for Solid Library

include(Common)

project(Solid)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier2.Solid
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
        DBus
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
        Solid
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Display
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Locale
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Network
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Network/connman
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Power
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Timedate
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools
)
