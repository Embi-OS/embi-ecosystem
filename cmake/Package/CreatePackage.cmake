include_guard(GLOBAL)

set(PACKAGE_CPACK_THREADS "0" CACHE STRING "Number of CPack compression threads; 0 lets CPack use all available cores.")
set(PACKAGE_IFW_ARCHIVE_FORMAT "7z" CACHE STRING "Qt Installer Framework archive format for component data.")
set_property(CACHE PACKAGE_IFW_ARCHIVE_FORMAT PROPERTY STRINGS 7z zip tar.gz tar.bz2 tar.xz)
set(PACKAGE_IFW_ARCHIVE_COMPRESSION "1" CACHE STRING "Qt Installer Framework archive compression level.")
set_property(CACHE PACKAGE_IFW_ARCHIVE_COMPRESSION PROPERTY STRINGS 0 1 3 5 7 9)

function(_package_sanitize_name OUTPUT VALUE)
    set(name "${VALUE}")
    string(REGEX REPLACE "[^A-Za-z0-9_.]" "_" name "${name}")
    string(REGEX REPLACE "_+" "_" name "${name}")
    string(REGEX REPLACE "^_+" "" name "${name}")
    string(REGEX REPLACE "_+$" "" name "${name}")
    if(NOT name)
        set(name Package)
    endif()

    set(${OUTPUT} "${name}" PARENT_SCOPE)
endfunction()

function(_package_sanitize_path OUTPUT VALUE)
    set(path "${VALUE}")
    string(REPLACE "\\" "-" path "${path}")
    string(REPLACE "/" "-" path "${path}")
    string(REGEX REPLACE "[[:space:]]+" "_" path "${path}")
    string(REGEX REPLACE "_+" "_" path "${path}")
    string(REGEX REPLACE "^_+" "" path "${path}")
    string(REGEX REPLACE "_+$" "" path "${path}")
    if(NOT path)
        set(path "${CMAKE_PROJECT_NAME}")
    endif()

    set(${OUTPUT} "${path}" PARENT_SCOPE)
endfunction()

function(_package_output_prefix OUTPUT VALUE)
    set(prefix "${VALUE}")
    if(prefix)
        string(REGEX REPLACE "^[\\/]+" "" prefix "${prefix}")
        string(REGEX REPLACE "[\\/]+$" "" prefix "${prefix}")
    endif()

    set(${OUTPUT} "${prefix}" PARENT_SCOPE)
endfunction()

function(_package_append_prefix OUTPUT BASE PREFIX)
    set(path "${BASE}")
    if(PREFIX)
        string(APPEND path "/${PREFIX}")
    endif()

    cmake_path(NORMAL_PATH path)
    set(${OUTPUT} "${path}" PARENT_SCOPE)
endfunction()

function(_package_platform TARGET OUTPUT)
    set(supported FALSE)
    set(extension "")
    set(cpack_program "")

    if(WIN32)
        set(supported TRUE)
        set(extension exe)
        set(cpack_program cpack.exe)
    elseif(UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT BOOT2QT AND NOT CMAKE_CROSSCOMPILING)
        set(supported TRUE)
        set(extension run)
        set(cpack_program cpack)
    endif()

    if(NOT supported)
        message(STATUS "[PACKAGE] Skipping ${TARGET}: unsupported platform ${CMAKE_SYSTEM_NAME}")
    endif()

    set(${OUTPUT} "${supported}" PARENT_SCOPE)
    set(package_installable_extension "${extension}" PARENT_SCOPE)
    set(package_cpack_program "${cpack_program}" PARENT_SCOPE)
endfunction()

function(_package_cpack_executable OUTPUT CPACK_PROGRAM)
    if(DEFINED CMAKE_CPACK_COMMAND AND EXISTS "${CMAKE_CPACK_COMMAND}")
        set(executable "${CMAKE_CPACK_COMMAND}")
    else()
        get_filename_component(cmake_bin_dir "${CMAKE_COMMAND}" DIRECTORY)
        set(executable "${cmake_bin_dir}/${CPACK_PROGRAM}")
    endif()

    set(${OUTPUT} "${executable}" PARENT_SCOPE)
endfunction()

function(_package_resolve_style_sheet OUTPUT CUSTOM DISABLED)
    if(DISABLED)
        set(${OUTPUT} "" PARENT_SCOPE)
        return()
    endif()

    if(CUSTOM)
        set(style_sheet "${CUSTOM}")
        if(IS_ABSOLUTE "${style_sheet}")
            cmake_path(NORMAL_PATH style_sheet)
        else()
            cmake_path(ABSOLUTE_PATH style_sheet
                BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                NORMALIZE
            )
        endif()
    else()
        set(style_sheet "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PackageStyle.qss")
        cmake_path(NORMAL_PATH style_sheet)
    endif()

    if(NOT EXISTS "${style_sheet}")
        message(FATAL_ERROR "Installer stylesheet not found: ${style_sheet}")
    endif()

    set(${OUTPUT} "${style_sheet}" PARENT_SCOPE)
endfunction()

macro(_package_find_ifw)
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

    set(package_ifw_found TRUE)
    if(NOT CPACK_IFW_BINARYCREATOR_EXECUTABLE OR NOT CPACK_IFW_INSTALLERBASE_EXECUTABLE)
        set(package_ifw_found FALSE)
    endif()
endmacro()

macro(_package_set_common_context TARGET)
    set(package_file_basename
        "${TARGET}_${CMAKE_PROJECT_VERSION}-${CMAKE_PROJECT_VERSION_SUFFIX}_${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
    )

    set(package_vendor "${CMAKE_PROJECT_COMPANY}")
    if(NOT package_vendor)
        set(package_vendor "${CMAKE_PROJECT_NAME}")
    endif()

    set(package_description "${CMAKE_PROJECT_DESCRIPTION}")
    if(NOT package_description)
        set(package_description "${TARGET}")
    endif()

    _package_sanitize_path(package_vendor_path "${package_vendor}")
    _package_sanitize_name(package_ifw_name "${TARGET}")
endmacro()

macro(_package_set_install_directories INSTALL_DIRECTORY)
    if(UNIX AND NOT APPLE)
        set(PACKAGECONFIG_IFW_TARGET_DIRECTORY "@HomeDir@/${INSTALL_DIRECTORY}")
        set(PACKAGECONFIG_IFW_ADMIN_TARGET_DIRECTORY "@ApplicationsDir@/${INSTALL_DIRECTORY}")
        set(PACKAGECONFIG_IFW_ADMIN_TARGET_DIRECTORY_ELEMENT
            "    <AdminTargetDir>${PACKAGECONFIG_IFW_ADMIN_TARGET_DIRECTORY}</AdminTargetDir>"
        )
    else()
        set(PACKAGECONFIG_IFW_TARGET_DIRECTORY "@ApplicationsDir@/${INSTALL_DIRECTORY}")
        set(PACKAGECONFIG_IFW_ADMIN_TARGET_DIRECTORY "")
        set(PACKAGECONFIG_IFW_ADMIN_TARGET_DIRECTORY_ELEMENT "")
    endif()
endmacro()

macro(_package_set_ifw_options)
    set(PACKAGECONFIG_CPACK_THREADS "${PACKAGE_CPACK_THREADS}")
    set(PACKAGECONFIG_IFW_ARCHIVE_FORMAT "${PACKAGE_IFW_ARCHIVE_FORMAT}")
    set(PACKAGECONFIG_IFW_ARCHIVE_COMPRESSION "${PACKAGE_IFW_ARCHIVE_COMPRESSION}")

    set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_STYLE "${arg_INSTALLER_WIZARD_STYLE}")
    if(NOT PACKAGECONFIG_IFW_PACKAGE_WIZARD_STYLE)
        set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_STYLE Classic)
    endif()

    set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH "${arg_INSTALLER_WIZARD_DEFAULT_WIDTH}")
    if(NOT PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH)
        set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_WIDTH 960)
    endif()

    set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT "${arg_INSTALLER_WIZARD_DEFAULT_HEIGHT}")
    if(NOT PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT)
        set(PACKAGECONFIG_IFW_PACKAGE_WIZARD_DEFAULT_HEIGHT 600)
    endif()

    set(PACKAGECONFIG_IFW_PACKAGE_TITLE_COLOR "${arg_INSTALLER_TITLE_COLOR}")
    if(NOT PACKAGECONFIG_IFW_PACKAGE_TITLE_COLOR)
        set(PACKAGECONFIG_IFW_PACKAGE_TITLE_COLOR "#FFFFFF")
    endif()

    _package_resolve_style_sheet(
        PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET
        "${arg_INSTALLER_STYLE_SHEET}"
        "${arg_NO_INSTALLER_STYLE}"
    )

endmacro()

macro(_package_set_ifw_tools)
    set(PACKAGECONFIG_IFW_BINARYCREATOR_EXECUTABLE "${CPACK_IFW_BINARYCREATOR_EXECUTABLE}")
    set(PACKAGECONFIG_IFW_INSTALLERBASE_EXECUTABLE "${CPACK_IFW_INSTALLERBASE_EXECUTABLE}")
    set(PACKAGECONFIG_IFW_REPOGEN_EXECUTABLE "${CPACK_IFW_REPOGEN_EXECUTABLE}")
    set(PACKAGECONFIG_IFW_ARCHIVEGEN_EXECUTABLE "${CPACK_IFW_ARCHIVEGEN_EXECUTABLE}")
    set(PACKAGECONFIG_IFW_DEVTOOL_EXECUTABLE "${CPACK_IFW_DEVTOOL_EXECUTABLE}")

    set(PACKAGECONFIG_IFW_ROOT "")
    if(DEFINED CPACK_IFW_ROOT)
        set(PACKAGECONFIG_IFW_ROOT "${CPACK_IFW_ROOT}")
    endif()

    set(PACKAGECONFIG_QTIFWDIR "")
    if(DEFINED ENV{QTIFWDIR})
        set(PACKAGECONFIG_QTIFWDIR "$ENV{QTIFWDIR}")
    endif()
endmacro()

function(install_optional_runtime_files TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "DESTINATION;DIRECTORY" "FILES")

    set(runtime_files "")
    foreach(runtime_file IN LISTS arg_FILES)
        if(IS_ABSOLUTE "${runtime_file}")
            list(APPEND runtime_files "${runtime_file}")
        else()
            list(APPEND runtime_files "${arg_DIRECTORY}/${runtime_file}")
        endif()
    endforeach()

    install(FILES ${runtime_files}
        DESTINATION ${arg_DESTINATION}
        COMPONENT ${TARGET}
        OPTIONAL
    )
endfunction()

function(create_package)
    set(args_options
        NO_PORTABLE
        NO_INSTALLER
        NO_INSTALLER_STYLE
    )
    set(args_single
        TARGET
        OUTPUT_DIRECTORY_PREFIX
        INSTALLER_STYLE_SHEET
        INSTALLER_TITLE_COLOR
        INSTALLER_WIZARD_DEFAULT_HEIGHT
        INSTALLER_WIZARD_DEFAULT_WIDTH
        INSTALLER_WIZARD_STYLE
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "${args_options}" "${args_single}" "")

    if(NOT EXEC_CPACK)
        return()
    endif()

    _package_platform("${arg_TARGET}" package_supported_platform)
    if(NOT package_supported_platform)
        return()
    endif()

    set_property(GLOBAL APPEND PROPERTY CREATE_PACKAGE_COMPONENTS "${arg_TARGET}")
    _package_output_prefix(output_directory_prefix "${arg_OUTPUT_DIRECTORY_PREFIX}")
    _package_set_common_context("${arg_TARGET}")
    _package_append_prefix(portable_root_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Portable" "${output_directory_prefix}")
    _package_append_prefix(packages_root_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Packages" "${output_directory_prefix}")
    _package_append_prefix(packages_work_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Cpack" "${output_directory_prefix}")

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

    if(WIN32 AND TARGET Qt${QT_VERSION_MAJOR}::Core)
        install_optional_runtime_files(${arg_TARGET}
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            DIRECTORY "$<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Core>"
            FILES
                libmariadb.dll
                libmysql.dll
        )
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
        _package_find_ifw()

        if(NOT package_ifw_found)
            message(STATUS "[PACKAGE] Skipping installer for ${arg_TARGET}: Qt Installer Framework not found")
        else()
            _package_cpack_executable(cpack_executable "${package_cpack_program}")
            _package_set_ifw_options()
            _package_set_ifw_tools()

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
            set(PACKAGECONFIG_IFW_PACKAGE_NAME "${package_ifw_name}")
            set(PACKAGECONFIG_PACKAGE_VENDOR "${package_vendor}")
            set(PACKAGECONFIG_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
            set(PACKAGECONFIG_PACKAGE_DESCRIPTION "${package_description}")
            set(PACKAGECONFIG_PACKAGE_DIRECTORY "${package_cpack_output_directory}")
            set(PACKAGECONFIG_PACKAGE_FILE_NAME "${package_file_basename}")
            set(PACKAGECONFIG_PACKAGE_INSTALL_DIRECTORY "${package_install_directory}")
            _package_set_install_directories("${package_install_directory}")
            set(PACKAGECONFIG_COMPONENT "${arg_TARGET}")
            set(PACKAGECONFIG_COMPONENT_METADATA "")
            set(PACKAGECONFIG_INSTALL_CMAKE_PROJECTS
                "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};${arg_TARGET};/"
            )
            set(PACKAGECONFIG_MONOLITHIC_INSTALL TRUE)

            configure_file(
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PackageConfig.cmake.in"
                "${package_cpack_config_file}"
                @ONLY
            )

            set(package_installer_dependencies
                ${arg_TARGET}
                "${package_cpack_config_file}"
            )
            if(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET)
                list(APPEND package_installer_dependencies "${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET}")
            endif()

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
                DEPENDS ${package_installer_dependencies}
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

function(create_package_bundle)
    set(args_options
        NO_INSTALLER_STYLE
    )
    set(args_single
        TARGET
        OUTPUT_DIRECTORY_PREFIX
        INSTALLER_STYLE_SHEET
        INSTALLER_TITLE_COLOR
        INSTALLER_WIZARD_DEFAULT_HEIGHT
        INSTALLER_WIZARD_DEFAULT_WIDTH
        INSTALLER_WIZARD_STYLE
    )
    set(args_multi
        COMPONENTS
    )

    cmake_parse_arguments(PARSE_ARGV 0 arg "${args_options}" "${args_single}" "${args_multi}")

    if(NOT EXEC_CPACK)
        return()
    endif()

    if(NOT arg_TARGET)
        set(arg_TARGET "${CMAKE_PROJECT_NAME}")
    endif()

    _package_platform("${arg_TARGET}" package_supported_platform)
    if(NOT package_supported_platform)
        return()
    endif()

    _package_find_ifw()
    if(NOT package_ifw_found)
        message(STATUS "[PACKAGE] Skipping bundle ${arg_TARGET}: Qt Installer Framework not found")
        return()
    endif()

    get_property(registered_components GLOBAL PROPERTY CREATE_PACKAGE_COMPONENTS)
    set(package_components "")
    set(package_component_package_names "")
    set(package_component_package_files "")

    foreach(component IN LISTS arg_COMPONENTS)
        set(component_target "${component}")
        if(NOT TARGET "${component_target}" AND TARGET "A${component}")
            set(component_target "A${component}")
        endif()

        if("${component_target}" IN_LIST registered_components)
            list(APPEND package_components "${component_target}")
        else()
            message(STATUS "[PACKAGE] Bundle ${arg_TARGET} skipping ${component}: package component not registered")
        endif()
    endforeach()

    if(NOT package_components)
        message(FATAL_ERROR "create_package_bundle ${arg_TARGET} has no registered package components")
    endif()

    _package_output_prefix(output_directory_prefix "${arg_OUTPUT_DIRECTORY_PREFIX}")
    _package_set_common_context("${arg_TARGET}")
    _package_set_ifw_options()
    _package_append_prefix(packages_root_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Packages" "${output_directory_prefix}")
    _package_append_prefix(packages_work_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}_Cpack" "${output_directory_prefix}")

    set(package_install_directory "${package_vendor_path}/${arg_TARGET}")
    set(package_cpack_root_directory "${CMAKE_BINARY_DIR}/.cpack")
    set(package_output_file "${packages_root_directory}/${package_file_basename}.${package_installable_extension}")
    set(package_cpack_work_directory "${packages_work_directory}/${arg_TARGET}")
    set(package_cpack_config_file "${package_cpack_root_directory}/${arg_TARGET}.xml")
    set(package_cpack_config_directory "${package_cpack_work_directory}/config")
    set(package_cpack_packages_directory "${package_cpack_work_directory}/packages")
    set(package_cpack_output_file
        "${package_cpack_work_directory}/${package_file_basename}.${package_installable_extension}"
    )
    cmake_path(NORMAL_PATH package_cpack_root_directory)
    cmake_path(NORMAL_PATH package_cpack_work_directory)
    cmake_path(NORMAL_PATH package_cpack_config_file)
    cmake_path(NORMAL_PATH package_cpack_config_directory)
    cmake_path(NORMAL_PATH package_cpack_packages_directory)
    cmake_path(NORMAL_PATH package_output_file)
    cmake_path(NORMAL_PATH package_cpack_output_file)

    file(MAKE_DIRECTORY "${package_cpack_root_directory}")

    set(PACKAGECONFIG_PACKAGE_NAME "${arg_TARGET}")
    set(PACKAGECONFIG_IFW_PACKAGE_NAME "${package_ifw_name}")
    set(PACKAGECONFIG_PACKAGE_VENDOR "${package_vendor}")
    set(PACKAGECONFIG_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
    set(PACKAGECONFIG_PACKAGE_INSTALL_DIRECTORY "${package_install_directory}")
    _package_set_install_directories("${package_install_directory}")

    if(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET)
        get_filename_component(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET_NAME
            "${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET}"
            NAME
        )
        set(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET_ELEMENT
            "    <StyleSheet>${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET_NAME}</StyleSheet>"
        )
    else()
        set(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET_ELEMENT "")
    endif()

    configure_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PackageInstallerConfig.xml.in"
        "${package_cpack_config_file}"
        @ONLY
    )

    string(TIMESTAMP package_release_date "%Y-%m-%d" UTC)
    foreach(component_target IN LISTS package_components)
        _package_sanitize_name(component_package_name "${package_ifw_name}.${component_target}")
        set(component_package_file "${package_cpack_root_directory}/${arg_TARGET}.${component_target}.package.xml")
        set(PACKAGECOMPONENT_DISPLAY_NAME "${component_target}")
        set(PACKAGECOMPONENT_DESCRIPTION "${component_target}")
        set(PACKAGECOMPONENT_NAME "${component_package_name}")
        set(PACKAGECOMPONENT_VERSION "${CMAKE_PROJECT_VERSION}")
        set(PACKAGECOMPONENT_RELEASE_DATE "${package_release_date}")

        configure_file(
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/PackageComponent.xml.in"
            "${component_package_file}"
            @ONLY
        )

        list(APPEND package_component_package_names "${component_package_name}")
        list(APPEND package_component_package_files "${component_package_file}")
    endforeach()

    set(package_commands "")
    list(APPEND package_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory "${packages_root_directory}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${package_cpack_work_directory}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${package_cpack_config_directory}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${package_cpack_packages_directory}"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${package_output_file}"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${package_cpack_output_file}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${package_cpack_config_file}"
            "${package_cpack_config_directory}/config.xml"
    )

    if(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET)
        list(APPEND package_commands
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET}"
                "${package_cpack_config_directory}/${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET_NAME}"
        )
    endif()

    list(LENGTH package_components package_component_count)
    math(EXPR package_component_last_index "${package_component_count} - 1")
    foreach(component_index RANGE 0 ${package_component_last_index})
        list(GET package_components ${component_index} component_target)
        list(GET package_component_package_names ${component_index} component_package_name)
        list(GET package_component_package_files ${component_index} component_package_file)
        set(component_package_directory "${package_cpack_packages_directory}/${component_package_name}")

        list(APPEND package_commands
            COMMAND ${CMAKE_COMMAND} -E make_directory "${component_package_directory}/meta"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${component_package_directory}/data/${component_target}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${component_package_file}"
                "${component_package_directory}/meta/package.xml"
            COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
                --config $<CONFIG>
                --component "${component_target}"
                --prefix "${component_package_directory}/data/${component_target}"
        )
    endforeach()

    set(package_bundle_dependencies
        ${package_components}
        "${package_cpack_config_file}"
        ${package_component_package_files}
    )
    if(PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET)
        list(APPEND package_bundle_dependencies "${PACKAGECONFIG_IFW_PACKAGE_STYLE_SHEET}")
    endif()

    message(STATUS "[PACKAGE] ${arg_TARGET} bundle installer -> ${package_output_file}")

    add_custom_command(
        OUTPUT "${package_output_file}"
        ${package_commands}
        COMMAND "${CPACK_IFW_BINARYCREATOR_EXECUTABLE}"
            --archive-format "${PACKAGECONFIG_IFW_ARCHIVE_FORMAT}"
            --compression "${PACKAGECONFIG_IFW_ARCHIVE_COMPRESSION}"
            -c "${package_cpack_config_directory}/config.xml"
            -p "${package_cpack_packages_directory}"
            "${package_cpack_output_file}"
        COMMAND ${CMAKE_COMMAND} -E rename
            "${package_cpack_output_file}"
            "${package_output_file}"
        DEPENDS ${package_bundle_dependencies}
        COMMENT "[PACKAGE] Creating ${package_output_file}"
        VERBATIM
    )

    add_custom_target("${arg_TARGET}_installable" ALL DEPENDS "${package_output_file}")
    add_custom_target("${arg_TARGET}_package" ALL DEPENDS "${arg_TARGET}_installable")
endfunction()
