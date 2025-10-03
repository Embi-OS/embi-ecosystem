
function(srcFileRecurseRelative FILES)

    set(args_option "")
    set(args_single PATH)
    set(args_multi)

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    unset(FILES_IN)

    if(DEFINED arg_PATH)
        set(PATH "${arg_PATH}/*.h;*.hpp;*.cpp;*.ui")
    else()
        set(PATH "*.h;*.hpp;*.cpp;*.ui")
    endif()

    file(GLOB_RECURSE FILES_IN LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH})

    set(${FILES} ${FILES_IN} PARENT_SCOPE)

endfunction()

function(qmlFileRecurseRelative FILES)

    set(args_option "")
    set(args_single PATH)
    set(args_multi)

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    unset(FILES_IN)

    if(DEFINED arg_PATH)
        set(PATH "${arg_PATH}/*.qml;*.js;*.mjs")
    else()
        set(PATH "*.qml;*.js;*.mjs")
    endif()

    file(GLOB_RECURSE FILES_IN LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH})

    set(${FILES} ${FILES_IN} PARENT_SCOPE)

endfunction()

function(allFileRecurseRelative FILES PATH)

    unset(FILES_IN)

    file(GLOB_RECURSE FILES_IN LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${PATH})

    set(${FILES} ${FILES_IN} PARENT_SCOPE)

endfunction()
