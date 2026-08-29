include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/Boot2Qt.cmake")

function(embi_setup_platform)

    set(args_single
        DEPLOY_PREFIX
        BOOT2QT_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${args_single}" "")
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    set(output_prefix "${CMAKE_BINARY_DIR}/../${CMAKE_PROJECT_NAME}")

    set(CMAKE_OUTPUT_PREFIX "${output_prefix}" PARENT_SCOPE)

    if(BOOT2QT)
        message(NOTICE "CMake Building for Boot2Qt")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_prefix}_Boot2Qt" PARENT_SCOPE)
        set(QT_DEPLOY_PREFIX "${arg_DEPLOY_PREFIX}" PARENT_SCOPE)
    elseif(EMSCRIPTEN)
        message(NOTICE "CMake Building for WASM")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_prefix}_WASM" PARENT_SCOPE)
        set(QT_DEPLOY_PREFIX "" PARENT_SCOPE)
    else()
        if(WIN32)
            message(NOTICE "CMake Building for Windows")
        elseif(APPLE)
            message(NOTICE "CMake Building for macOS")
        else()
            message(NOTICE "CMake Building for Linux")
        endif()

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_prefix}" PARENT_SCOPE)
        set(QT_DEPLOY_PREFIX "${arg_DEPLOY_PREFIX}" PARENT_SCOPE)
    endif()

    if(BOOT2QT)
        add_compile_definitions(Q_OS_BOOT2QT)
    endif()

    if(RASPBERRY_PI)
        add_compile_definitions(Q_DEVICE_RASPBERRY_PI)
    endif()

    if(APALIS_IMX8)
        add_compile_definitions(Q_DEVICE_APALIS_IMX8)
    endif()

    if(arg_BOOT2QT_PREFIX)
        add_compile_definitions(B2QT_PREFIX="${arg_BOOT2QT_PREFIX}")
    endif()

endfunction()
