# CMakeLists.txt for QDefs Library

include(Common)

project(QDefs)

srcFileRecurseRelative(ALL_SRC_FILES)

embi_add_library(${PROJECT_NAME}
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        QDefs
        README.md
        LICENSE
)
