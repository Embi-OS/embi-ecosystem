# CMakeLists.txt for QUtils Library

include(Common)

project(QUtils)

srcFileRecurseRelative(ALL_SRC_FILES)

embi_add_qml_module(${PROJECT_NAME}
    URI Eco.Tier1.Utils
    STATIC
    PRIVATE_QT_LIBRARIES
        Concurrent
    LIBRARIES
        QDefs
        QJsonVariant
    SRC_FILES
        ${ALL_SRC_FILES}
    DOC_FILES
        QUtils
        README.md
        LICENSE
)
