include_guard(GLOBAL)

macro(embi_setup_qt_project)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_setup_qt_project does not accept arguments")
    endif()

    set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)
    set(BUILD_SHARED_LIBS OFF)

    find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Qml Quick)
    set(QT_VERSION_MAJOR 6)
    set(QT_VERSION "${Qt6_VERSION}")
    qt_standard_project_setup(REQUIRES 6.8)

    foreach(qt_policy_name QTP0001 QTP0003 QTP0004 QTP0005)
        qt_policy(SET ${qt_policy_name} NEW)
    endforeach()

    set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qt/qml")
    set(QT_QML_GENERATE_QMLLS_INI ON)
    set(QML_IMPORT_PATH "${QT_QML_OUTPUT_DIRECTORY}" CACHE STRING "Qt Creator extra qml import paths" FORCE)

endmacro()

function(embi_disable_qt_creator_maintenance_provider)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_disable_qt_creator_maintenance_provider does not accept arguments")
    endif()

    set(QT_CREATOR_ENABLE_MAINTENANCE_TOOL_PROVIDER OFF CACHE BOOL
        "Disable Qt Creator MaintenanceTool dependency provider" FORCE
    )

endfunction()

function(embi_detect_qt_creator_run)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_detect_qt_creator_run does not accept arguments")
    endif()

    if(DEFINED ENV{QTC_RUN})
        message(NOTICE "CMake is running under Qt Creator")
        set(QT_CREATOR_RUN TRUE PARENT_SCOPE)
    else()
        message(NOTICE "CMake is running outside Qt Creator")
        set(QT_CREATOR_RUN FALSE PARENT_SCOPE)
    endif()

endfunction()
