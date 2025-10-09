string(TOLOWER "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE_lower)

# Initialize variables to OFF before conditional checks
set(RASPBERRY_PI OFF)
set(APALIS_IMX8 OFF)
set(BOOT2QT OFF)

if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "raspberrypi")
    set(RASPBERRY_PI ON)
    set(BOOT2QT ON)
endif()

if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "apalis-imx8")
    set(APALIS_IMX8 ON)
    set(BOOT2QT ON)
endif()

if(CMAKE_TOOLCHAIN_FILE_lower MATCHES "boot2qt")
    set(BOOT2QT ON)
endif()

set(RASPBERRY_PI ${RASPBERRY_PI} CACHE BOOL "")
set(APALIS_IMX8 ${APALIS_IMX8} CACHE BOOL "")
set(BOOT2QT ${BOOT2QT} CACHE BOOL "")
