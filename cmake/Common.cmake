# Common cmake file

include(FileRecurseRelative)

cmake_minimum_required(VERSION 3.22)

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

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS ${QT_LIBRARY})
    endforeach()

    set(LIB_TYPE)
    if(arg_STATIC)
        set(LIB_TYPE STATIC)
    elseif(arg_SHARED)
        set(LIB_TYPE SHARED)
    endif()

    qt_add_library(${NAME}
        ${LIB_TYPE}
        ${arg_SRC_FILES}
        ${arg_DOC_FILES}
    )

    target_compile_definitions(${NAME} PRIVATE
        QT_MESSAGELOGCONTEXT
        QT_DISABLE_DEPRECATED_UP_TO=0x060500
        QT_DEPRECATED_WARNINGS
    )

    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Gui)
    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Qml)

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        set(QT_TARGET "Qt${QT_VERSION_MAJOR}::${QT_LIBRARY}")
        if(TARGET ${QT_TARGET})
            target_link_libraries(${NAME} PRIVATE ${QT_TARGET})
        else()
            message(STATUS "Skipping unavailable Qt module: ${QT_TARGET}")
        endif()
    endforeach()

    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

endfunction()

function(embi_add_qml_module NAME)

    set(args_option
        STATIC
        SHARED
        ENABLE_TYPE_COMPILER
        NO_GENERATE_EXTRA_QMLDIRS
    )
    set(args_single URI)
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

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS ${QT_LIBRARY})
    endforeach()

    foreach(QML_SINGLETON ${arg_QML_SINGLETONS})
        set_source_files_properties(${QML_SINGLETON} PROPERTIES QT_QML_SINGLETON_TYPE TRUE )
    endforeach()

    set(LIB_TYPE)
    if(arg_STATIC)
        set(LIB_TYPE STATIC)
    elseif(arg_SHARED)
        set(LIB_TYPE SHARED)
    endif()

    set(ENABLE_TYPE_COMPILER)
    if(arg_ENABLE_TYPE_COMPILER)
        set(ENABLE_TYPE_COMPILER ENABLE_TYPE_COMPILER)
    endif()

    set(NO_GENERATE_EXTRA_QMLDIRS)
    if(arg_NO_GENERATE_EXTRA_QMLDIRS)
        set(NO_GENERATE_EXTRA_QMLDIRS NO_GENERATE_EXTRA_QMLDIRS)
    endif()

    qt_add_qml_module(${NAME}
        URI ${arg_URI}
        ${LIB_TYPE}
        ${ENABLE_TYPE_COMPILER}
        ${NO_GENERATE_EXTRA_QMLDIRS}
        IMPORT_PATH ${CMAKE_BINARY_DIR}/qt/qml
        DEPENDENCIES
            QtCore
            QtQml
            QtQuick
            ${arg_QML_DEPENDENCIES}
        IMPORTS
            ${arg_QML_IMPORTS}
        SOURCES
            ${arg_SRC_FILES}
            ${arg_DOC_FILES}
        QML_FILES
            ${arg_QML_FILES}
    )

    if(QMLCACHEGEN_STATIC)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--static")
    endif()
    if(QMLCACHEGEN_DIRECT)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--direct-calls")
    endif()
    if(QMLCACHEGEN_VERBOSE)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--verbose")
    endif()
    if(QMLCACHEGEN_BYTECODE)
        list(APPEND QMLCACHEGEN_ARGUMENTS "--only-bytecode")
    endif()
    list(JOIN QMLCACHEGEN_ARGUMENTS ";" QMLCACHEGEN_ARGUMENT)
    set_target_properties(${NAME} PROPERTIES QT_QMLCACHEGEN_ARGUMENTS "${QMLCACHEGEN_ARGUMENT}")

    target_compile_definitions(${NAME} PRIVATE
        QT_MESSAGELOGCONTEXT
        QT_DISABLE_DEPRECATED_UP_TO=0x060500
        QT_DEPRECATED_WARNINGS
    )

    qt_add_resources(${NAME} "${NAME}_images"
        FILES
            ${arg_IMG_FILES}
    )

    qt_add_resources(${NAME} "${NAME}_fonts"
        FILES
            ${arg_FONTS_FILES}
    )

    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Gui)
    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Qml)
    target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Quick)

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        set(QT_TARGET "Qt${QT_VERSION_MAJOR}::${QT_LIBRARY}")
        if(TARGET ${QT_TARGET})
            target_link_libraries(${NAME} PRIVATE ${QT_TARGET})
        else()
            message(STATUS "Skipping unavailable Qt module: ${QT_TARGET}")
        endif()
    endforeach()

    target_link_libraries(${NAME} PUBLIC ${arg_LIBRARIES})

    foreach(PLUGIN ${arg_PLUGINS})
        target_link_libraries(${NAME} PUBLIC ${PLUGIN})
        target_link_libraries(${NAME} PUBLIC ${PLUGIN}plugin)
    endforeach()

    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${NAME} PUBLIC ${arg_DIRECTORIES})
    target_include_directories(${NAME} PRIVATE ${arg_PRIVATE_DIRECTORIES})

endfunction()

function(embi_add_executable NAME)

    set(args_option
        ENABLE_TYPE_COMPILER
    )
    set(args_single
        URI
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

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS ${QT_LIBRARY})
    endforeach()

    if(DEFINED arg_URI)
        set(ENABLE_TYPE_COMPILER)
        if(arg_ENABLE_TYPE_COMPILER)
            set(ENABLE_TYPE_COMPILER ENABLE_TYPE_COMPILER)
        endif()

        set(NO_GENERATE_EXTRA_QMLDIRS)
        if(arg_NO_GENERATE_EXTRA_QMLDIRS)
            set(NO_GENERATE_EXTRA_QMLDIRS NO_GENERATE_EXTRA_QMLDIRS)
        endif()

        qt_add_executable(${NAME} main.cpp)
        qt_add_qml_module(${NAME}
            URI ${arg_URI}
            ${ENABLE_TYPE_COMPILER}
            ${NO_GENERATE_EXTRA_QMLDIRS}
            IMPORT_PATH ${CMAKE_BINARY_DIR}/qt/qml
            DEPENDENCIES
                QtCore
                QtQml
                QtQuick
                ${arg_QML_DEPENDENCIES}
            IMPORTS
                ${arg_QML_IMPORTS}
            SOURCES
                ${arg_SRC_FILES}
                ${arg_DOC_FILES}
            QML_FILES
                ${arg_QML_FILES}
        )

        if(QMLCACHEGEN_STATIC)
            list(APPEND QMLCACHEGEN_ARGUMENTS "--static")
        endif()
        if(QMLCACHEGEN_DIRECT)
            list(APPEND QMLCACHEGEN_ARGUMENTS "--direct-calls")
        endif()
        if(QMLCACHEGEN_VERBOSE)
            list(APPEND QMLCACHEGEN_ARGUMENTS "--verbose")
        endif()
        list(JOIN QMLCACHEGEN_ARGUMENTS ";" QMLCACHEGEN_ARGUMENT)
        set_target_properties(${NAME} PROPERTIES QT_QMLCACHEGEN_ARGUMENTS "${QMLCACHEGEN_ARGUMENT}")

        qt_add_resources(${NAME} "${NAME}_images"
            FILES
                ${arg_IMG_FILES}
        )

    else()
        qt_add_executable(${NAME} ${arg_SRC_FILES})
    endif()

    set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

    target_compile_definitions(${NAME} PRIVATE
        QT_MESSAGELOGCONTEXT
        QT_DISABLE_DEPRECATED_UP_TO=0x060500
        QT_DEPRECATED_WARNINGS
    )

    target_link_libraries(${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
    target_link_libraries(${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
    target_link_libraries(${NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Quick)

    foreach(QT_LIBRARY ${arg_QT_LIBRARIES})
        target_link_libraries(${NAME} PUBLIC Qt${QT_VERSION_MAJOR}::${QT_LIBRARY})
    endforeach()

    foreach(QT_LIBRARY ${arg_PRIVATE_QT_LIBRARIES})
        set(QT_TARGET "Qt${QT_VERSION_MAJOR}::${QT_LIBRARY}")
        if(TARGET ${QT_TARGET})
            target_link_libraries(${NAME} PRIVATE ${QT_TARGET})
        else()
            message(STATUS "Skipping unavailable Qt module: ${QT_TARGET}")
        endif()
    endforeach()

    target_link_libraries(${NAME} PRIVATE ${arg_LIBRARIES})

    foreach(PLUGIN ${arg_PLUGINS})
        target_link_libraries(${NAME} PRIVATE ${PLUGIN})
        target_link_libraries(${NAME} PRIVATE ${PLUGIN}plugin)
    endforeach()

    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${NAME} PRIVATE ${arg_DIRECTORIES})
    target_include_directories(${NAME} PRIVATE ${arg_PRIVATE_DIRECTORIES})

    if(BOOT2QT)
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_OUTPUT_DIRECTORY_PREFIX}")
    else()
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_OUTPUT_DIRECTORY_PREFIX}/${NAME}")
    endif()

    set_target_properties(${NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    )

    if (BOOT2QT AND ${NAME} IN_LIST DEPLOYABLE_APPS)
        message(NOTICE "[DEPLOYABLE_APPS] Installing ${APP_NAME} in ${QT_DEPLOY_PREFIX}")
        set(INSTALL_DIR "${QT_DEPLOY_PREFIX}")
        install(TARGETS ${NAME}
            RUNTIME DESTINATION "${INSTALL_DIR}"
            BUNDLE DESTINATION "${INSTALL_DIR}"
            LIBRARY DESTINATION "${INSTALL_DIR}"
        )
    endif()

endfunction()

function(embi_add_application NAME)

    set(args_option "")
    set(args_single
        APP_NAME
        LIB_NAME
        URI
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

    qt_add_executable(${APP_NAME} main.cpp)
    set_property(TARGET ${APP_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

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
        LIBRARIES ${arg_LIBRARIES}
        PLUGINS ${arg_PLUGINS}
        DIRECTORIES ${arg_DIRECTORIES}
        PRIVATE_DIRECTORIES ${arg_PRIVATE_DIRECTORIES}
    )

    if(NOT ${APP_NAME} MATCHES ${LIB_NAME})
        target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
        target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
        target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Qml)
        target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Quick)
        target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME})
        target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME}plugin)
    endif()

    if(BOOT2QT)
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_OUTPUT_DIRECTORY_PREFIX}")
    else()
        set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_OUTPUT_DIRECTORY_PREFIX}/${NAME}")
    endif()

    set_target_properties(${APP_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    )

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_definitions(${APP_NAME} PRIVATE QT_QML_DEBUG)
    endif()

    if (BOOT2QT AND ${NAME} IN_LIST DEPLOYABLE_APPS)
        message(NOTICE "[DEPLOYABLE_APPS] Installing ${APP_NAME} in ${QT_DEPLOY_PREFIX}")
        set(INSTALL_DIR "${QT_DEPLOY_PREFIX}")
        install(TARGETS ${APP_NAME}
            RUNTIME DESTINATION "${INSTALL_DIR}"
            BUNDLE DESTINATION "${INSTALL_DIR}"
            LIBRARY DESTINATION "${INSTALL_DIR}"
        )
    endif()

endfunction()
