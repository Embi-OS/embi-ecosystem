# CMakeLists.txt for QProxyModel Library

include(Common)

project(QProxyModel)

srcFileRecurseRelative(ALL_SRC_FILES)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier1.ProxyModel
    STATIC
    LIBRARIES
        QDefs
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        QProxyModel
        README.md
        LICENSE
    PRIVATE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/filters
        ${CMAKE_CURRENT_SOURCE_DIR}/proxyroles
        ${CMAKE_CURRENT_SOURCE_DIR}/sorters
)
