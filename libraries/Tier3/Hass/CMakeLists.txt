include(Common)

project(Hass)

allFileRecurseRelative(ALL_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
allFileRecurseRelative(ALL_QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/qml/*)
allFileRecurseRelative(ALL_IMG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/images/*)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier3.Hass
    STATIC
    PLUGINS
        Axion
    LIBRARIES
        QModels
        Rest
    QML_IMPORTS
        Eco.Tier2.Rest
    QML_DEPENDENCIES
        Eco.Tier1.Models
        Eco.Tier2.Rest
        Eco.Tier3.Axion
    SRC_FILES
        ${ALL_SRC_FILES}
    QML_FILES
        ${ALL_QML_FILES}
    IMG_FILES
        ${ALL_IMG_FILES}
    DOC_FILES
        Hass
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_DIRECTORIES
        ${CMAKE_SOURCE_DIR}/libraries/Tier2/Rest/src/RestModel
)
