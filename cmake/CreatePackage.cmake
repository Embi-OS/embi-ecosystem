include_guard(GLOBAL)

function(create_package)
    set(args_options
        NO_PORTABLE
        NO_INSTALLER
    )
    set(args_single
        TARGET
        OUTPUT_DIRECTORY_PREFIX
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "${args_options}" "${args_single}" "")

    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown/unexpected arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()
    if(NOT arg_TARGET)
        message(FATAL_ERROR "create_package requires TARGET")
    endif()
    if(arg_NO_PORTABLE AND arg_NO_INSTALLER)
        message(FATAL_ERROR "create_package cannot disable both portable and installer outputs")
    endif()
    if(NOT EXEC_CPACK)
        return()
    endif()
    if(NOT TARGET ${arg_TARGET})
        message(FATAL_ERROR "Packaging target '${arg_TARGET}' does not exist")
    endif()

    get_target_property(target_type ${arg_TARGET} TYPE)
    if(NOT target_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "Packaging target '${arg_TARGET}' must be an executable")
    endif()

    if(WIN32)
        set(package_supported_platform TRUE)
        set(package_installable_extension "exe")
        set(package_cpack_program "cpack.exe")
    elseif(UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT BOOT2QT AND NOT CMAKE_CROSSCOMPILING)
        set(package_supported_platform TRUE)
        set(package_installable_extension "run")
        set(package_cpack_program "cpack")
    else()
        set(package_supported_platform FALSE)
    endif()

    if(NOT package_supported_platform)
        message(STATUS "[PACKAGE] Skipping ${arg_TARGET}: unsupported platform ${CMAKE_SYSTEM_NAME}")
        return()
    endif()

    set(output_directory_prefix "${arg_OUTPUT_DIRECTORY_PREFIX}")
    if(output_directory_prefix)
        string(REGEX REPLACE "^/+" "" output_directory_prefix "${output_directory_prefix}")
        string(REGEX REPLACE "/+$" "" output_directory_prefix "${output_directory_prefix}")
    endif()

    qt_generate_deploy_qml_app_script(
        TARGET ${arg_TARGET}
        OUTPUT_SCRIPT deploy_script
    )

    install(TARGETS ${arg_TARGET}
        COMPONENT ${arg_TARGET}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(SCRIPT "${deploy_script}"
        COMPONENT ${arg_TARGET}
    )

    set(package_file_basename
        "${arg_TARGET}_${CMAKE_PROJECT_VERSION}-${CMAKE_PROJECT_VERSION_SUFFIX}_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
    )

    set(package_vendor "${CMAKE_PROJECT_COMPANY}")
    if(NOT package_vendor)
        set(package_vendor "${CMAKE_PROJECT_NAME}")
    endif()

    set(package_vendor_path "${package_vendor}")
    string(REPLACE "\\" "-" package_vendor_path "${package_vendor_path}")
    string(REPLACE "/" "-" package_vendor_path "${package_vendor_path}")
    string(REGEX REPLACE "[[:space:]]+" "_" package_vendor_path "${package_vendor_path}")
    string(REGEX REPLACE "_+" "_" package_vendor_path "${package_vendor_path}")
    string(REGEX REPLACE "^_+" "" package_vendor_path "${package_vendor_path}")
    string(REGEX REPLACE "_+$" "" package_vendor_path "${package_vendor_path}")
    if(NOT package_vendor_path)
        set(package_vendor_path "${CMAKE_PROJECT_NAME}")
    endif()

    set(package_description "${CMAKE_PROJECT_DESCRIPTION}")
    if(NOT package_description)
        set(package_description "${arg_TARGET}")
    endif()

    set(portable_root_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Portable")
    set(packages_root_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Packages")
    set(packages_work_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Cpack")

    if(output_directory_prefix)
        string(APPEND portable_root_directory "/${output_directory_prefix}")
        string(APPEND packages_root_directory "/${output_directory_prefix}")
        string(APPEND packages_work_directory "/${output_directory_prefix}")
    endif()

    cmake_path(NORMAL_PATH portable_root_directory)
    cmake_path(NORMAL_PATH packages_root_directory)
    cmake_path(NORMAL_PATH packages_work_directory)

    if(DEFINED CMAKE_CPACK_COMMAND AND EXISTS "${CMAKE_CPACK_COMMAND}")
        set(cpack_executable "${CMAKE_CPACK_COMMAND}")
    else()
        get_filename_component(cmake_bin_dir "${CMAKE_COMMAND}" DIRECTORY)
        set(cpack_executable "${cmake_bin_dir}/${package_cpack_program}")
    endif()

    set(package_dependencies "")

    if(NOT arg_NO_PORTABLE)
        set(portable_directory "${portable_root_directory}/${package_file_basename}")
        set(portable_archive "${portable_root_directory}/${package_file_basename}.zip")
        set(portable_stamp_file "${portable_directory}/.stamp")
        cmake_path(NORMAL_PATH portable_directory)
        cmake_path(NORMAL_PATH portable_archive)

        message(STATUS "[PACKAGE] ${arg_TARGET} portable -> ${portable_archive}")

        add_custom_command(
            OUTPUT
                "${portable_stamp_file}"
                "${portable_archive}"
            COMMAND ${CMAKE_COMMAND} -E rm -rf "${portable_directory}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${portable_root_directory}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${portable_directory}"
            COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
                --config $<CONFIG>
                --component "${arg_TARGET}"
                --prefix "${portable_directory}"
            COMMAND ${CMAKE_COMMAND} -E rm -f "${portable_archive}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${portable_root_directory}"
                ${CMAKE_COMMAND} -E tar cf "${portable_archive}" --format=zip "${package_file_basename}"
            COMMAND ${CMAKE_COMMAND} -E touch "${portable_stamp_file}"
            DEPENDS ${arg_TARGET}
            COMMENT "[PORTABLE] Creating ${portable_archive}"
            VERBATIM
        )

        add_custom_target("${arg_TARGET}_portable" ALL DEPENDS "${portable_archive}")
        list(APPEND package_dependencies "${arg_TARGET}_portable")
    endif()

    if(NOT arg_NO_INSTALLER)
        set(package_install_directory "")

        foreach(ifw_variable IN ITEMS
            CPACK_IFW_BINARYCREATOR_EXECUTABLE
            CPACK_IFW_INSTALLERBASE_EXECUTABLE
            CPACK_IFW_REPOGEN_EXECUTABLE
            CPACK_IFW_ARCHIVEGEN_EXECUTABLE
            CPACK_IFW_DEVTOOL_EXECUTABLE
        )
            if(DEFINED ${ifw_variable} AND "${${ifw_variable}}" MATCHES "-NOTFOUND$")
                unset(${ifw_variable} CACHE)
                unset(${ifw_variable})
            endif()
        endforeach()

        include(CPackIFW)

        if(NOT CPACK_IFW_BINARYCREATOR_EXECUTABLE OR NOT CPACK_IFW_INSTALLERBASE_EXECUTABLE)
            message(STATUS "[PACKAGE] Skipping installer for ${arg_TARGET}: Qt Installer Framework not found")
        else()
            set(package_install_directory "${package_vendor_path}/${arg_TARGET}")

            set(package_cpack_root_directory "${CMAKE_BINARY_DIR}/.cpack")
            set(package_output_file "${packages_root_directory}/${package_file_basename}.${package_installable_extension}")
            set(package_cpack_work_directory "${packages_work_directory}/${arg_TARGET}")
            set(package_cpack_config_file "${package_cpack_root_directory}/${arg_TARGET}.cmake")
            set(package_cpack_output_directory "${package_cpack_work_directory}")
            set(package_cpack_output_file
                "${package_cpack_output_directory}/${package_file_basename}.${package_installable_extension}"
            )
            cmake_path(NORMAL_PATH package_cpack_root_directory)
            cmake_path(NORMAL_PATH package_cpack_work_directory)
            cmake_path(NORMAL_PATH package_cpack_config_file)
            cmake_path(NORMAL_PATH package_output_file)
            cmake_path(NORMAL_PATH package_cpack_output_file)

            file(MAKE_DIRECTORY "${package_cpack_root_directory}")

            set(PACKAGECONFIG_PACKAGE_NAME "${arg_TARGET}")
            set(PACKAGECONFIG_PACKAGE_VENDOR "${package_vendor}")
            set(PACKAGECONFIG_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
            set(PACKAGECONFIG_PACKAGE_DESCRIPTION "${package_description}")
            set(PACKAGECONFIG_PACKAGE_DIRECTORY "${package_cpack_output_directory}")
            set(PACKAGECONFIG_PACKAGE_FILE_NAME "${package_file_basename}")
            set(PACKAGECONFIG_PACKAGE_INSTALL_DIRECTORY "${package_install_directory}")
            set(PACKAGECONFIG_COMPONENT "${arg_TARGET}")
            set(PACKAGECONFIG_INSTALL_CMAKE_PROJECTS
                "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};${arg_TARGET};/"
            )
            set(PACKAGECONFIG_IFW_BINARYCREATOR_EXECUTABLE "${CPACK_IFW_BINARYCREATOR_EXECUTABLE}")
            set(PACKAGECONFIG_IFW_INSTALLERBASE_EXECUTABLE "${CPACK_IFW_INSTALLERBASE_EXECUTABLE}")
            set(PACKAGECONFIG_IFW_REPOGEN_EXECUTABLE "${CPACK_IFW_REPOGEN_EXECUTABLE}")
            set(PACKAGECONFIG_IFW_ARCHIVEGEN_EXECUTABLE "${CPACK_IFW_ARCHIVEGEN_EXECUTABLE}")
            set(PACKAGECONFIG_IFW_DEVTOOL_EXECUTABLE "${CPACK_IFW_DEVTOOL_EXECUTABLE}")
            if(DEFINED CPACK_IFW_ROOT)
                set(PACKAGECONFIG_IFW_ROOT "${CPACK_IFW_ROOT}")
            else()
                set(PACKAGECONFIG_IFW_ROOT "")
            endif()
            if(DEFINED ENV{QTIFWDIR})
                set(PACKAGECONFIG_QTIFWDIR "$ENV{QTIFWDIR}")
            else()
                set(PACKAGECONFIG_QTIFWDIR "")
            endif()

            configure_file(
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PackageConfig.cmake.in"
                "${package_cpack_config_file}"
                @ONLY
            )

            message(STATUS "[PACKAGE] ${arg_TARGET} installer -> ${package_output_file}")

            add_custom_command(
                OUTPUT "${package_output_file}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${packages_root_directory}"
                COMMAND ${CMAKE_COMMAND} -E rm -rf "${package_cpack_work_directory}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${package_cpack_work_directory}"
                COMMAND ${CMAKE_COMMAND} -E rm -f "${package_output_file}"
                COMMAND ${CMAKE_COMMAND} -E rm -f "${package_cpack_output_file}"
                COMMAND "${cpack_executable}" --config "${package_cpack_config_file}"
                COMMAND ${CMAKE_COMMAND} -E rename
                    "${package_cpack_output_file}"
                    "${package_output_file}"
                DEPENDS
                    ${arg_TARGET}
                    "${package_cpack_config_file}"
                COMMENT "[PACKAGE] Creating ${package_output_file}"
                VERBATIM
            )

            add_custom_target("${arg_TARGET}_installable" ALL DEPENDS "${package_output_file}")
            list(APPEND package_dependencies "${arg_TARGET}_installable")
        endif()
    endif()

    if(package_dependencies)
        add_custom_target("${arg_TARGET}_package" ALL DEPENDS ${package_dependencies})
    endif()
endfunction()
