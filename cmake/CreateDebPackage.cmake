find_package(Qt6 REQUIRED COMPONENTS Core)

function(create_deb_package TARGET)

    if(NOT EXEC_CPACK)
        return()
    endif()

    set(args_option "")
    set(args_single "")
    set(args_multi
        OUTPUT_DIRECTORY_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 1 arg
        "${args_option}" "${args_single}" "${args_multi}"
    )
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    set(deploy_tool_options_arg "")
    if(BOOT2QT)
        return ()
    elseif(EMSCRIPTEN)
        return ()
    elseif(APPLE)
        return ()
    elseif(UNIX)
        set(CPACK_GENERATOR "DEB")
        set(PACKAGE_EXT "deb")
    elseif(WIN32)
        set(deploy_tool_options_arg "")  # copy compiler runtime DLLs
        set(CPACK_GENERATOR "ZIP")
        set(PACKAGE_EXT "zip")
    endif()

    # Install the executable into "${CMAKE_INSTALL_PREFIX}/bin".
    message(STATUS "[DEB] Install the ${TARGET} executable into ${CMAKE_INSTALL_PREFIX}/bin")
    install(TARGETS ${TARGET}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    message(STATUS "[DEB] Generate the deployment script for the target ${TARGET}")
    qt_generate_deploy_qml_app_script(
        TARGET ${TARGET}
        OUTPUT_SCRIPT deploy_script
        MACOS_BUNDLE_POST_BUILD
        NO_UNSUPPORTED_PLATFORM_ERROR
        DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
        DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
    )

    # ---------------------------
    # CPack configuration
    
    if(NOT DEFINED SANITIZED_PROJECT_VERSION)
        set(SANITIZED_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}")
    endif()

    set(CPACK_PACKAGE_NAME ${TARGET})
    set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_VENDOR "${CMAKE_PROJECT_COMPANY}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${SANITIZED_PROJECT_VERSION}_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_VERBATIM_VARIABLES ON)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${QT_DEPLOY_PREFIX}/${TARGET}")

    if(CPACK_GENERATOR STREQUAL "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CMAKE_PROJECT_MAINTAINER}")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgcc-s1, libxcb-cursor0")
    endif()

    set(CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_CPackConfig.cmake")
    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_CPackSourceConfig.cmake")
    set(OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Deb/${arg_OUTPUT_DIRECTORY_PREFIX}/${CPACK_PACKAGE_FILE_NAME}")
    set(CPACK_INSTALLED_DIRECTORIES "${OUTPUT_DIRECTORY};/")
    include(CPack)

    message(STATUS "[DEB] After configuration, you will find the cpack config file at: ${CPACK_OUTPUT_CONFIG_FILE}")

    # ---------------------------
    # This makes the install step happen automatically as part of the build.
    message(STATUS "[DEB] Call the deployment script for ${TARGET} on cmake --install")
    install(SCRIPT ${deploy_script})

    add_custom_command(
        OUTPUT "${OUTPUT_DIRECTORY}/.stamp"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_CURRENT_BINARY_DIR}" --prefix "${OUTPUT_DIRECTORY}" --config $<CONFIG>
        COMMAND ${CMAKE_COMMAND} -E touch "${OUTPUT_DIRECTORY}/.stamp"
        DEPENDS ${TARGET}
        COMMENT "[RUNDIR] Installing and deploying ${TARGET} into ${OUTPUT_DIRECTORY}"
        VERBATIM
    )

    add_custom_target("${TARGET}_rundir" ALL
        DEPENDS "${OUTPUT_DIRECTORY}/.stamp"
    )

    message(STATUS "[DEB] After building, you will find a ready-to-run directory at: ${OUTPUT_DIRECTORY}")

    return()
    # ---------------------------
    # Run cpack at build time
    find_program(CPACK_EXEC cpack REQUIRED)
    set(PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.${PACKAGE_EXT}")
    set(PACKAGE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_FILE_NAME}")
    set(OUTPUT_PACKAGE_FILE "${OUTPUT_DIRECTORY}/../${PACKAGE_FILE_NAME}")

    add_custom_command(
        OUTPUT "${OUTPUT_PACKAGE_FILE}"
        COMMAND "${CPACK_EXEC}" -G "${CPACK_GENERATOR}" --config "${CPACK_OUTPUT_CONFIG_FILE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIRECTORY}/.."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PACKAGE_FILE}" "${OUTPUT_DIRECTORY}/.."
        DEPENDS "${TARGET}_rundir"
        COMMENT "[CPACK] Creating ${OUTPUT_PACKAGE_FILE} using CPack"
        VERBATIM
    )

    add_custom_target("${TARGET}_deb" ALL
        DEPENDS "${OUTPUT_PACKAGE_FILE}"
    )

    message(STATUS "[CPACK] After build, package will be at ${OUTPUT_PACKAGE_FILE}")

endfunction()

