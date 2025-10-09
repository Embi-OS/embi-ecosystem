if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

if(NOT GIT_TAG)
    set(GIT_TAG "00000000")
endif()

set(GIT_TAG_HEX "0x${GIT_TAG}")
set(GIT_TAG "${GIT_TAG}" CACHE STRING "")
