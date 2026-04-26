string(TOLOWER "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE_lower)

option(BOOT2QT "Build for Boot2Qt runtime" OFF)
option(RASPBERRY_PI "Build for Raspberry Pi target" OFF)
option(APALIS_IMX8 "Build for Apalis iMX8 target" OFF)

# Fallback heuristics for SDK/local builds. Yocto recipe builds typically use a
# generic toolchain.cmake filename, so callers should prefer passing these
# options explicitly when target details matter.
if(NOT BOOT2QT AND NOT RASPBERRY_PI AND NOT APALIS_IMX8)

    if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "raspberrypi")
        set(RASPBERRY_PI ON CACHE BOOL "" FORCE)
        set(BOOT2QT ON CACHE BOOL "" FORCE)
    endif()

    if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "apalis-imx8")
        set(APALIS_IMX8 ON CACHE BOOL "" FORCE)
        set(BOOT2QT ON CACHE BOOL "" FORCE)
    endif()

    if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "boot2qt")
        set(BOOT2QT ON CACHE BOOL "" FORCE)
    endif()

endif()

if(RASPBERRY_PI OR APALIS_IMX8)
    set(BOOT2QT ON CACHE BOOL "" FORCE)
endif()