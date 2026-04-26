# CMakeLists.txt for Solid Library

include(Common)
include(TryLinkLibrary)

project(Solid)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Solid
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
        DBus
    PLUGINS
        Axion
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    DOC_FILES
        Solid
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dbus
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fstab
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hostname
        ${CMAKE_CURRENT_SOURCE_DIR}/src/locale
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network/connman
        ${CMAKE_CURRENT_SOURCE_DIR}/src/swupdate
        ${CMAKE_CURRENT_SOURCE_DIR}/src/system
        ${CMAKE_CURRENT_SOURCE_DIR}/src/timedate
)

target_try_link_libraries(${PROJECT_NAME} swupdate SWUPDATE)
