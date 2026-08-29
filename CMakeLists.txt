cmake_minimum_required(VERSION 3.24)

## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(QtSetup)
include(ProjectOptions)
include(PlatformSetup)
embi_detect_qt_creator_run()
embi_setup_project_options()
embi_disable_qt_creator_maintenance_provider()

include(ProjectMetadata)
embi_setup_default_project_metadata(
    VERSION 0.0.0
    VERSION_SUFFIX "alpha"
    VERSION_CODENAME ""
    DESCRIPTION ""
)

project(Embi-Ecosystem
    VERSION ${DEFAULT_PROJECT_VERSION}
    DESCRIPTION "${DEFAULT_PROJECT_DESCRIPTION}"
    LANGUAGES CXX)

include(GNUInstallDirs)
embi_set_project_metadata(
    VERSION_SUFFIX "${DEFAULT_PROJECT_VERSION_SUFFIX}"
    VERSION_CODENAME "${DEFAULT_PROJECT_VERSION_CODENAME}"
    COMPANY "Embi-OS"
    COMPANY_ADDRESS "Switzerland"
    COMPANY_ZIPCODE ""
    COMPANY_CONTACT ""
    WEBSITE "github.com/Embi-OS"
    DOMAIN "embi.os"
    COPYRIGHT ""
    MAINTAINER "Embi-OS <github.com/Embi-OS>"
)
message(NOTICE "Setup project ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION}-${CMAKE_PROJECT_VERSION_SUFFIX}")

embi_setup_compiler_defaults()
embi_setup_qt_project()

embi_setup_platform(
    DEPLOY_PREFIX "/Embi"
    BOOT2QT_PREFIX "/etc/default/b2qt"
)
embi_setup_optimization_flags()
embi_setup_debug_sanitizers()

include(GitTag)
include(ProjectDiagnostics)

embi_add_project_compile_definitions(
    APP_CONTROLLER_COMMAND "appcontroller"
)

add_subdirectory(libraries)
add_subdirectory(applications)

embi_print_project_diagnostics()
