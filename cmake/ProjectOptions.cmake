include_guard(GLOBAL)

macro(_embi_option NAME DESCRIPTION DEFAULT_VALUE)

    if(NOT DEFINED ${NAME})
        option(${NAME} "${DESCRIPTION}" ${DEFAULT_VALUE})
    endif()

endmacro()

function(embi_setup_project_options)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_setup_project_options does not accept arguments")
    endif()

    _embi_option(EXEC_CPACK "Build package targets with CPack" OFF)
    _embi_option(ENABLE_OPTIMIZATION "Enable size and IPO-oriented release optimizations" OFF)
    _embi_option(ENABLE_ASAN "Enable AddressSanitizer for debug builds" ON)
    _embi_option(DISABLE_SAMPLES "Disable sample applications" OFF)
    _embi_option(DISABLE_TESTS "Disable test targets" OFF)
    _embi_option(TRANSLATION_LUPDATE "Run lupdate as part of the default build" OFF)

endfunction()

function(embi_setup_compiler_defaults)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_setup_compiler_defaults does not accept arguments")
    endif()

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release PARENT_SCOPE)
    endif()

    set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
    set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)

    string(APPEND CMAKE_CXX_FLAGS " -Wall -pedantic-errors")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)

endfunction()

function(embi_setup_optimization_flags)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_setup_optimization_flags does not accept arguments")
    endif()

    set(ENABLE_TARGET_IPO OFF PARENT_SCOPE)
    if(NOT ENABLE_OPTIMIZATION)
        return()
    endif()

    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Build without interprocedural optimization: MinGW GCC LTO is unstable with Qt on Windows")
        set(enable_target_ipo OFF)
    else()
        set(enable_target_ipo ON)
    endif()

    if(BOOT2QT)
        set(size_flag -Os)
    elseif(EMSCRIPTEN)
        set(size_flag -Oz)
    else()
        set(size_flag -Os)
    endif()

    message(STATUS "Build with optimization")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " ${size_flag}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" PARENT_SCOPE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ${enable_target_ipo} PARENT_SCOPE)
    set(ENABLE_TARGET_IPO ${enable_target_ipo} PARENT_SCOPE)

endfunction()

function(embi_setup_debug_sanitizers)

    if(ARGC GREATER 0)
        message(FATAL_ERROR "embi_setup_debug_sanitizers does not accept arguments")
    endif()

    if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()

endfunction()
