# TryLinkLibrary cmake file

function(target_try_link_libraries PROJECT_NAME LIBRARY PREFIX)

    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(${PREFIX} QUIET ${LIBRARY})
    endif()

    if(${PREFIX}_FOUND)
        message(STATUS "${LIBRARY} version ${${PREFIX}_VERSION} found. Building with ${PREFIX} support.")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${${PREFIX}_LIBRARIES})
        target_compile_definitions(${PROJECT_NAME} PRIVATE ${PREFIX}_FOUND)
    else()
        find_library(${PREFIX}_LIB NAMES ${LIBRARY})
        if(${PREFIX}_LIB)
            message(STATUS "${LIBRARY} found. Building with ${PREFIX} support.")
            target_link_libraries(${PROJECT_NAME} PRIVATE ${${PREFIX}_LIB})
            target_compile_definitions(${PROJECT_NAME} PRIVATE ${PREFIX}_FOUND)
        else()
            message(STATUS "${LIBRARY} not found. Building ${PROJECT_NAME} without ${PREFIX} support.")
        endif()
    endif()

endfunction()
