# CMakeLists.txt for QModels Library

include(Common)

project(QModels)

srcFileRecurseRelative(ALL_SRC_FILES)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier1.Models
    STATIC
    LIBRARIES
        QJsonVariant
        QDefs
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        QModels
        README.md
        LICENSE
    DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/syncable
)
