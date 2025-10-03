# CMakeLists.txt for Axion Library

include(Common)
include(GenerateIconsClass)

project(Axion)

generateIconsClass(FiletypesIcons FILETYPES_ICONS_FILES
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/images/filetypes/*
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    PREFIX /images/filetypes
)
generateIconsClass(MaterialIcons MATERIAL_ICONS_FILES
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/images/material/*
    OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    PREFIX /images/material
)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_IMG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/*)
allFileRecurseRelative(ALL_FONTS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Axion
    STATIC
    QT_LIBRARIES
        QuickTemplates2
        QuickControls2
        QuickControls2Impl
        Charts
        Svg
        VirtualKeyboard
        Widgets
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QsLog
        QDefs
        QUtils
        QModels
        QProxyModel
    PLUGINS
        Unit
    QML_IMPORTS
        QtQuick.Controls.Material
        QtQuick.Layouts
    QML_DEPENDENCIES
        QtQuick.Effects
        QtQuick.Templates
        QtQuick.Controls.Material.impl
        QtCharts
        QtQuick.VirtualKeyboard
        Eco.Tier1.Utils
        Eco.Tier1.Models
        Eco.Tier1.ProxyModel
        Eco.Tier2.Unit
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    QML_SINGLETONS
        qml/JS.qml
        qml/Style.qml
    IMG_FILES
        ${ALL_IMG_FILES}
    FONTS_FILES
        ${ALL_FONTS_FILES}
    DOC_FILES
        Axion
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/forms
        ${CMAKE_CURRENT_SOURCE_DIR}/src/forms/validators
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graphs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/helpers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models
        ${CMAKE_CURRENT_SOURCE_DIR}/src/steps
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tables
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tables/columnobjects
        ${CMAKE_CURRENT_SOURCE_DIR}/src/templates
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

if(NOT EMSCRIPTEN)
    allFileRecurseRelative(ALL_KEYBOARD_FILES ${CMAKE_CURRENT_SOURCE_DIR}/keyboard/*)
    qt_add_resources(${PROJECT_NAME} "${PROJECT_NAME}_keyboard"
        BASE keyboard
        PREFIX /qt-project.org/imports/QtQuick/VirtualKeyboard
        FILES
            ${ALL_KEYBOARD_FILES}
    )
endif()
