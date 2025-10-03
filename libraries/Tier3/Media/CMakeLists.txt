# CMakeLists.txt for Media Library

include(Common)

project(Media)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_IMG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Media
    STATIC
    QT_LIBRARIES
        Multimedia
    PRIVATE_QT_LIBRARIES
        Concurrent
    PLUGINS
        Axion
    QML_IMPORTS
        QtMultimedia
    QML_DEPENDENCIES
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    IMG_FILES
        ${ALL_IMG_FILES}
    DOC_FILES
        Media
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
