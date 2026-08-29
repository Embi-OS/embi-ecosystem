# Common cmake file

include(FileRecurseRelative)

cmake_minimum_required(VERSION 3.22)

macro(_embi_find_qt_libraries REQUIRED_LIBRARIES QUIET_LIBRARIES)

    foreach(QT_LIBRARY IN LISTS ${REQUIRED_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY IN LISTS ${QUIET_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS ${QT_LIBRARY})
    endforeach()

endmacro()

macro(_embi_resolve_library_type OUTPUT USE_STATIC USE_SHARED)

    set(${OUTPUT})
    if(${USE_STATIC})
        set(${OUTPUT} STATIC)
    elseif(${USE_SHARED})
        set(${OUTPUT} SHARED)
    endif()

endmacro()

function(_embi_target_compile_definitions TARGET)

    target_compile_definitions(${TARGET} PRIVATE
        QT_MESSAGELOGCONTEXT
        QT_DISABLE_DEPRECATED_UP_TO=0x060500
        QT_DEPRECATED_WARNINGS
    )

endfunction()

function(_embi_target_link_qt_libraries TARGET VISIBILITY)

    set(QT_TARGETS)
    foreach(QT_LIBRARY ${ARGN})
        list(APPEND QT_TARGETS Qt${QT_VERSION_MAJOR}::${QT_LIBRARY})
    endforeach()

    if(QT_TARGETS)
        target_link_libraries(${TARGET} ${VISIBILITY} ${QT_TARGETS})
    endif()

endfunction()

function(_embi_target_link_optional_qt_libraries TARGET VISIBILITY)

    foreach(QT_LIBRARY ${ARGN})
        set(QT_TARGET "Qt${QT_VERSION_MAJOR}::${QT_LIBRARY}")
        if(TARGET ${QT_TARGET})
            target_link_libraries(${TARGET} ${VISIBILITY} ${QT_TARGET})
        else()
            message(STATUS "Skipping unavailable Qt module: ${QT_TARGET}")
        endif()
    endforeach()

endfunction()

function(_embi_target_link_libraries TARGET VISIBILITY)

    if(ARGN)
        target_link_libraries(${TARGET} ${VISIBILITY} ${ARGN})
    endif()

endfunction()

function(_embi_target_link_plugins TARGET VISIBILITY)

    foreach(PLUGIN ${ARGN})
        target_link_libraries(${TARGET} ${VISIBILITY} ${PLUGIN})
        target_link_libraries(${TARGET} ${VISIBILITY} ${PLUGIN}plugin)
    endforeach()

endfunction()

macro(_embi_set_qmlcachegen_defaults PREFIX)

    if(NOT DEFINED ${PREFIX}_QMLCACHEGEN_STATIC)
        set(${PREFIX}_QMLCACHEGEN_STATIC ON)
    endif()
    if(NOT DEFINED ${PREFIX}_QMLCACHEGEN_DIRECT)
        set(${PREFIX}_QMLCACHEGEN_DIRECT OFF)
    endif()
    if(NOT DEFINED ${PREFIX}_QMLCACHEGEN_BYTECODE)
        set(${PREFIX}_QMLCACHEGEN_BYTECODE OFF)
    endif()
    if(NOT DEFINED ${PREFIX}_QMLCACHEGEN_VERBOSE)
        set(${PREFIX}_QMLCACHEGEN_VERBOSE OFF)
        if(QT_CREATOR_RUN)
            set(${PREFIX}_QMLCACHEGEN_VERBOSE ON)
        endif()
    endif()

endmacro()

function(_embi_set_qmlcachegen_arguments TARGET USE_STATIC USE_DIRECT USE_BYTECODE USE_VERBOSE)

    set(QMLCACHEGEN_ARGUMENTS)
    if(USE_STATIC)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--static")
    endif()
    if(USE_DIRECT)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--direct-calls")
    endif()
    if(USE_VERBOSE)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--verbose")
    endif()
    if(USE_BYTECODE)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--only-bytecode")
    endif()

    list(JOIN QMLCACHEGEN_ARGUMENTS ";" QMLCACHEGEN_ARGUMENT)
    set_target_properties(${TARGET} PROPERTIES QT_QMLCACHEGEN_ARGUMENTS "${QMLCACHEGEN_ARGUMENT}")

endfunction()

function(_embi_set_qml_singletons)

    foreach(QML_SINGLETON ${ARGN})
        set_source_files_properties(${QML_SINGLETON} PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
    endforeach()

endfunction()

function(_embi_add_resource_files TARGET RESOURCE_NAME USE_BIG_RESOURCES)

    if(NOT ARGN)
        return()
    endif()

    set(BIG_RESOURCES)
    if(USE_BIG_RESOURCES)
        set(BIG_RESOURCES BIG_RESOURCES)
    endif()

    qt_add_resources(${TARGET} "${RESOURCE_NAME}"
        ${BIG_RESOURCES}
        FILES
            ${ARGN}
    )

endfunction()

macro(_embi_add_qml_module_target
    TARGET
    URI
    LIB_TYPE
    USE_TYPE_COMPILER
    USE_NO_EXTRA_QMLDIRS
    QML_DEPENDENCIES
    QML_IMPORTS
    SRC_FILES
    DOC_FILES
    QML_FILES
)

    set(_embi_enable_type_compiler)
    if(${USE_TYPE_COMPILER})
        set(_embi_enable_type_compiler ENABLE_TYPE_COMPILER)
    endif()

    set(_embi_no_generate_extra_qmldirs)
    if(${USE_NO_EXTRA_QMLDIRS})
        set(_embi_no_generate_extra_qmldirs NO_GENERATE_EXTRA_QMLDIRS)
    endif()

    qt_add_qml_module(${TARGET}
        URI ${URI}
        ${LIB_TYPE}
        ${_embi_enable_type_compiler}
        ${_embi_no_generate_extra_qmldirs}
        IMPORT_PATH ${CMAKE_BINARY_DIR}/qt/qml
        DEPENDENCIES
            QtCore
            QtQml
            QtQuick
            ${${QML_DEPENDENCIES}}
        IMPORTS
            ${${QML_IMPORTS}}
        SOURCES
            ${${SRC_FILES}}
            ${${DOC_FILES}}
        QML_FILES
            ${${QML_FILES}}
    )

endmacro()

function(_embi_add_executable_target TARGET)

    if(WIN32)
        qt_add_executable(${TARGET} WIN32 ${ARGN})
    else()
        qt_add_executable(${TARGET} ${ARGN})
    endif()

endfunction()

function(_embi_enable_ipo TARGET)

    if(ENABLE_TARGET_IPO)
        set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()

endfunction()

function(_embi_set_executable_output_directory TARGET NAME OUTPUT_DIRECTORY_PREFIX)

    if(BOOT2QT)
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUT_DIRECTORY_PREFIX}")
    else()
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUT_DIRECTORY_PREFIX}/${NAME}")
    endif()

    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    )

endfunction()

function(embi_mark_boot2qt_deployable)

    set(args_single TARGET)

    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${args_single}" "")
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT arg_TARGET)
        message(FATAL_ERROR "embi_mark_boot2qt_deployable requires TARGET")
    endif()

    if(NOT TARGET "${arg_TARGET}")
        message(FATAL_ERROR "embi_mark_boot2qt_deployable target does not exist: ${arg_TARGET}")
    endif()

    get_target_property(target_type "${arg_TARGET}" TYPE)
    if(NOT target_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "embi_mark_boot2qt_deployable target must be an executable: ${arg_TARGET}")
    endif()

    set_property(TARGET "${arg_TARGET}" PROPERTY EMBI_BOOT2QT_DEPLOYABLE TRUE)

    get_property(boot2qt_deployable_targets GLOBAL PROPERTY EMBI_BOOT2QT_DEPLOYABLE_TARGETS)
    if(NOT "${arg_TARGET}" IN_LIST boot2qt_deployable_targets)
        set_property(GLOBAL APPEND PROPERTY EMBI_BOOT2QT_DEPLOYABLE_TARGETS "${arg_TARGET}")
    endif()

    if(BOOT2QT)
        message(STATUS "Installing ${arg_TARGET} in ${QT_DEPLOY_PREFIX}")
        install(TARGETS ${arg_TARGET}
            RUNTIME DESTINATION "${QT_DEPLOY_PREFIX}"
            BUNDLE DESTINATION "${QT_DEPLOY_PREFIX}"
            LIBRARY DESTINATION "${QT_DEPLOY_PREFIX}"
        )
    endif()

endfunction()

function(embi_exclude_from_translations)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_exclude_from_translations does not accept arguments")
    endif()

    set_property(GLOBAL APPEND PROPERTY EMBI_TRANSLATION_EXCLUDED_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")

endfunction()

function(embi_add_library NAME)

    set(args_option
        STATIC
        SHARED
    )
    set(args_single OUTPUT_DIRECTORY_PREFIX)
    set(args_multi
        QT_LIBRARIES
        PRIVATE_QT_LIBRARIES
        SRC_FILES
        DOC_FILES)

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    message(NOTICE "[PROJECT] Setup library ${NAME}")

    _embi_find_qt_libraries(arg_QT_LIBRARIES arg_PRIVATE_QT_LIBRARIES)
    _embi_resolve_library_type(LIB_TYPE arg_STATIC arg_SHARED)

    qt_add_library(${NAME}
        ${LIB_TYPE}
        ${arg_SRC_FILES}
        ${arg_DOC_FILES}
    )

    _embi_target_compile_definitions(${NAME})
    _embi_target_link_qt_libraries(${NAME} PUBLIC Core Gui Qml ${arg_QT_LIBRARIES})
    _embi_target_link_optional_qt_libraries(${NAME} PRIVATE ${arg_PRIVATE_QT_LIBRARIES})

    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()

function(embi_add_qml_module NAME)

    set(args_option
        STATIC
        SHARED
        ENABLE_TYPE_COMPILER
        NO_GENERATE_EXTRA_QMLDIRS
        BIG_RESOURCES
    )
    set(args_single
        URI
        QMLCACHEGEN_STATIC
        QMLCACHEGEN_DIRECT
        QMLCACHEGEN_BYTECODE
        QMLCACHEGEN_VERBOSE
    )
    set(args_multi
        QT_LIBRARIES
        PRIVATE_QT_LIBRARIES
        QML_DEPENDENCIES
        QML_IMPORTS
        SRC_FILES
        QML_FILES
        QML_SINGLETONS
        IMG_FILES
        FONTS_FILES
        DOC_FILES
        LIBRARIES
        PLUGINS
        DIRECTORIES
        PRIVATE_DIRECTORIES
    )

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    message(NOTICE "[PROJECT] Setup qml module ${arg_URI}")

    _embi_find_qt_libraries(arg_QT_LIBRARIES arg_PRIVATE_QT_LIBRARIES)
    _embi_set_qml_singletons(${arg_QML_SINGLETONS})
    _embi_resolve_library_type(LIB_TYPE arg_STATIC arg_SHARED)
    _embi_set_qmlcachegen_defaults(arg)

    _embi_add_qml_module_target(${NAME}
        "${arg_URI}"
        "${LIB_TYPE}"
        arg_ENABLE_TYPE_COMPILER
        arg_NO_GENERATE_EXTRA_QMLDIRS
        arg_QML_DEPENDENCIES
        arg_QML_IMPORTS
        arg_SRC_FILES
        arg_DOC_FILES
        arg_QML_FILES
    )

    _embi_set_qmlcachegen_arguments(${NAME}
        "${arg_QMLCACHEGEN_STATIC}"
        "${arg_QMLCACHEGEN_DIRECT}"
        "${arg_QMLCACHEGEN_BYTECODE}"
        "${arg_QMLCACHEGEN_VERBOSE}"
    )
    _embi_target_compile_definitions(${NAME})

    _embi_add_resource_files(${NAME} "${NAME}_images" "${arg_BIG_RESOURCES}" ${arg_IMG_FILES})
    _embi_add_resource_files(${NAME} "${NAME}_fonts" "${arg_BIG_RESOURCES}" ${arg_FONTS_FILES})
    _embi_target_link_qt_libraries(${NAME} PUBLIC Core Gui Qml Quick ${arg_QT_LIBRARIES})
    _embi_target_link_optional_qt_libraries(${NAME} PRIVATE ${arg_PRIVATE_QT_LIBRARIES})
    _embi_target_link_libraries(${NAME} PUBLIC ${arg_LIBRARIES})
    _embi_target_link_plugins(${NAME} PUBLIC ${arg_PLUGINS})

    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${NAME} PUBLIC ${arg_DIRECTORIES})
    target_include_directories(${NAME} PRIVATE ${arg_PRIVATE_DIRECTORIES})

endfunction()

function(embi_add_executable NAME)

    set(args_option
        ENABLE_TYPE_COMPILER
        BIG_RESOURCES
    )
    set(args_single
        URI
        QMLCACHEGEN_STATIC
        QMLCACHEGEN_DIRECT
        QMLCACHEGEN_BYTECODE
        QMLCACHEGEN_VERBOSE
    )
    set(args_multi
        QT_LIBRARIES
        PRIVATE_QT_LIBRARIES
        QML_DEPENDENCIES
        QML_IMPORTS
        SRC_FILES
        QML_FILES
        DOC_FILES
        IMG_FILES
        LIBRARIES
        PLUGINS
        DIRECTORIES
        PRIVATE_DIRECTORIES
        OUTPUT_DIRECTORY_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    message(NOTICE "[PROJECT] Setup executable ${NAME}")

    _embi_find_qt_libraries(arg_QT_LIBRARIES arg_PRIVATE_QT_LIBRARIES)
    _embi_set_qmlcachegen_defaults(arg)

    if(DEFINED arg_URI)
        _embi_add_executable_target(${NAME} main.cpp)
        _embi_add_qml_module_target(${NAME}
            "${arg_URI}"
            ""
            arg_ENABLE_TYPE_COMPILER
            arg_NO_GENERATE_EXTRA_QMLDIRS
            arg_QML_DEPENDENCIES
            arg_QML_IMPORTS
            arg_SRC_FILES
            arg_DOC_FILES
            arg_QML_FILES
        )

        _embi_set_qmlcachegen_arguments(${NAME}
            "${arg_QMLCACHEGEN_STATIC}"
            "${arg_QMLCACHEGEN_DIRECT}"
            "${arg_QMLCACHEGEN_BYTECODE}"
            "${arg_QMLCACHEGEN_VERBOSE}"
        )
        _embi_add_resource_files(${NAME} "${NAME}_images" "${arg_BIG_RESOURCES}" ${arg_IMG_FILES})

    else()
        _embi_add_executable_target(${NAME} ${arg_SRC_FILES})
    endif()

    _embi_enable_ipo(${NAME})
    _embi_target_compile_definitions(${NAME})
    _embi_target_link_qt_libraries(${NAME} PRIVATE Core Gui Qml Quick)
    _embi_target_link_qt_libraries(${NAME} PUBLIC ${arg_QT_LIBRARIES})
    _embi_target_link_optional_qt_libraries(${NAME} PRIVATE ${arg_PRIVATE_QT_LIBRARIES})
    _embi_target_link_libraries(${NAME} PRIVATE ${arg_LIBRARIES})
    _embi_target_link_plugins(${NAME} PRIVATE ${arg_PLUGINS})

    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${NAME} PRIVATE ${arg_DIRECTORIES})
    target_include_directories(${NAME} PRIVATE ${arg_PRIVATE_DIRECTORIES})

    _embi_set_executable_output_directory(${NAME} ${NAME} "${arg_OUTPUT_DIRECTORY_PREFIX}")
endfunction()

function(embi_add_application NAME)

    set(args_option
        BIG_RESOURCES
    )
    set(args_single
        APP_NAME
        LIB_NAME
        URI
        QMLCACHEGEN_STATIC
        QMLCACHEGEN_DIRECT
        QMLCACHEGEN_BYTECODE
        QMLCACHEGEN_VERBOSE
    )
    set(args_multi
        QT_LIBRARIES
        PRIVATE_QT_LIBRARIES
        QML_DEPENDENCIES
        QML_IMPORTS
        SRC_FILES
        QML_FILES
        QML_SINGLETONS
        DOC_FILES
        IMG_FILES
        LIBRARIES
        PLUGINS
        DIRECTORIES
        PRIVATE_DIRECTORIES
        OUTPUT_DIRECTORY_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    if(DEFINED arg_APP_NAME)
        set(APP_NAME ${arg_APP_NAME})
    else()
        set(APP_NAME ${NAME})
    endif()

    if(DEFINED arg_LIB_NAME)
        set(LIB_NAME ${arg_LIB_NAME})
        set(LIB_TYPE STATIC)
    else()
        set(LIB_NAME ${APP_NAME})
    endif()

    message(NOTICE "[PROJECT] Setup application ${NAME} (APP_NAME = ${APP_NAME}, LIB_NAME = ${LIB_NAME})")
    _embi_set_qmlcachegen_defaults(arg)

    set(BIG_RESOURCES)
    if(arg_BIG_RESOURCES)
        set(BIG_RESOURCES BIG_RESOURCES)
    endif()

    _embi_add_executable_target(${APP_NAME} main.cpp)
    _embi_enable_ipo(${APP_NAME})

    embi_add_qml_module(${LIB_NAME}
        URI ${LIB_NAME}
        ${LIB_TYPE}
        QT_LIBRARIES ${arg_QT_LIBRARIES}
        PRIVATE_QT_LIBRARIES ${arg_PRIVATE_QT_LIBRARIES}
        QML_DEPENDENCIES ${arg_QML_DEPENDENCIES}
        QML_IMPORTS ${arg_QML_IMPORTS}
        SRC_FILES ${arg_SRC_FILES}
        QML_FILES ${arg_QML_FILES}
        QML_SINGLETONS ${arg_QML_SINGLETONS}
        DOC_FILES ${arg_DOC_FILES}
        IMG_FILES ${arg_IMG_FILES}
        QMLCACHEGEN_STATIC ${arg_QMLCACHEGEN_STATIC}
        QMLCACHEGEN_DIRECT ${arg_QMLCACHEGEN_DIRECT}
        QMLCACHEGEN_BYTECODE ${arg_QMLCACHEGEN_BYTECODE}
        QMLCACHEGEN_VERBOSE ${arg_QMLCACHEGEN_VERBOSE}
        ${BIG_RESOURCES}
        LIBRARIES ${arg_LIBRARIES}
        PLUGINS ${arg_PLUGINS}
        DIRECTORIES ${arg_DIRECTORIES}
        PRIVATE_DIRECTORIES ${arg_PRIVATE_DIRECTORIES}
    )

    if(NOT ${APP_NAME} MATCHES ${LIB_NAME})
        _embi_target_link_qt_libraries(${APP_NAME} PRIVATE Core Gui Qml Quick)
        _embi_target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME} ${LIB_NAME}plugin)
    endif()

    _embi_set_executable_output_directory(${APP_NAME} ${NAME} "${arg_OUTPUT_DIRECTORY_PREFIX}")

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_definitions(${APP_NAME} PRIVATE QT_QML_DEBUG)
    endif()

endfunction()
