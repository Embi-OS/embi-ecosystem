include_guard(GLOBAL)

macro(embi_setup_default_project_metadata)

    set(args_single
        VERSION
        VERSION_SUFFIX
        VERSION_CODENAME
        DESCRIPTION
    )

    cmake_parse_arguments(arg "" "${args_single}" "" ${ARGN})
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT arg_VERSION)
        set(arg_VERSION 0.0.0)
    endif()

    if(NOT DEFINED arg_VERSION_SUFFIX)
        set(arg_VERSION_SUFFIX "alpha")
    endif()

    if(NOT DEFINED arg_VERSION_CODENAME)
        set(arg_VERSION_CODENAME "")
    endif()

    if(NOT DEFINED arg_DESCRIPTION)
        set(arg_DESCRIPTION "")
    endif()

    if(NOT DEFINED DEFAULT_PROJECT_VERSION)
        set(DEFAULT_PROJECT_VERSION ${arg_VERSION})
    endif()

    if(NOT DEFINED DEFAULT_PROJECT_VERSION_SUFFIX)
        set(DEFAULT_PROJECT_VERSION_SUFFIX "${arg_VERSION_SUFFIX}")
    endif()

    if(NOT DEFINED DEFAULT_PROJECT_VERSION_CODENAME)
        set(DEFAULT_PROJECT_VERSION_CODENAME "${arg_VERSION_CODENAME}")
    endif()

    if(NOT DEFINED DEFAULT_PROJECT_DESCRIPTION)
        set(DEFAULT_PROJECT_DESCRIPTION "${arg_DESCRIPTION}")
    endif()

endmacro()

function(embi_set_project_metadata)

    set(args_single
        VERSION_SUFFIX
        VERSION_CODENAME
        COMPANY
        COMPANY_ADDRESS
        COMPANY_ZIPCODE
        COMPANY_CONTACT
        WEBSITE
        DOMAIN
        COPYRIGHT
        MAINTAINER
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${args_single}" "")
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    set(CMAKE_PROJECT_VERSION_SUFFIX "${arg_VERSION_SUFFIX}" PARENT_SCOPE)
    set(CMAKE_PROJECT_VERSION_CODENAME "${arg_VERSION_CODENAME}" PARENT_SCOPE)
    set(CMAKE_PROJECT_COMPANY "${arg_COMPANY}" PARENT_SCOPE)
    set(CMAKE_PROJECT_COMPANY_ADDRESS "${arg_COMPANY_ADDRESS}" PARENT_SCOPE)
    set(CMAKE_PROJECT_COMPANY_ZIPCODE "${arg_COMPANY_ZIPCODE}" PARENT_SCOPE)
    set(CMAKE_PROJECT_COMPANY_CONTACT "${arg_COMPANY_CONTACT}" PARENT_SCOPE)
    set(CMAKE_PROJECT_WEBSITE "${arg_WEBSITE}" PARENT_SCOPE)
    set(CMAKE_PROJECT_DOMAIN "${arg_DOMAIN}" PARENT_SCOPE)
    set(CMAKE_PROJECT_COPYRIGHT "${arg_COPYRIGHT}" PARENT_SCOPE)
    set(CMAKE_PROJECT_MAINTAINER "${arg_MAINTAINER}" PARENT_SCOPE)

endfunction()

function(embi_add_project_compile_definitions)

    set(args_single
        APP_CONTROLLER_COMMAND
        BOOT2QT_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${args_single}" "")
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    add_compile_definitions(
        PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
        PROJECT_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
        PROJECT_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
        PROJECT_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}
        PROJECT_VERSION_SUFFIX="${CMAKE_PROJECT_VERSION_SUFFIX}"
        PROJECT_VERSION_CODENAME="${CMAKE_PROJECT_VERSION_CODENAME}"
        PROJECT_DESCRIPTION="${CMAKE_PROJECT_DESCRIPTION}"
        PROJECT_NAME="${CMAKE_PROJECT_NAME}"
        PROJECT_COMPANY="${CMAKE_PROJECT_COMPANY}"
        PROJECT_COMPANY_ADDRESS="${CMAKE_PROJECT_COMPANY_ADDRESS}"
        PROJECT_COMPANY_ZIPCODE="${CMAKE_PROJECT_COMPANY_ZIPCODE}"
        PROJECT_COMPANY_CONTACT="${CMAKE_PROJECT_COMPANY_CONTACT}"
        PROJECT_WEBSITE="${CMAKE_PROJECT_WEBSITE}"
        PROJECT_DOMAIN="${CMAKE_PROJECT_DOMAIN}"
        PROJECT_MAINTAINER="${CMAKE_PROJECT_MAINTAINER}"
        PROJECT_COPYRIGHT="${CMAKE_PROJECT_COPYRIGHT}"
    )

    if(arg_APP_CONTROLLER_COMMAND)
        add_compile_definitions(APPCONTROLLER_CMD="${arg_APP_CONTROLLER_COMMAND}")
    endif()

    if(arg_BOOT2QT_PREFIX)
        add_compile_definitions(B2QT_PREFIX="${arg_BOOT2QT_PREFIX}")
    endif()

    if(QT_CREATOR_RUN)
        add_compile_definitions(QT_CREATOR_RUN)
    endif()

endfunction()
