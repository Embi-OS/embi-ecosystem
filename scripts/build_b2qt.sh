#!/bin/sh
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer

QT_B2QT_KIT="raspberrypi-armv8"
B2QT_PREFIX_PATH="$QT_ROOT/$QT_VERSION/EmbiOS/$QT_B2QT_KIT/toolchain/sysroots/x86_64-pokysdk-linux"
B2QT_MAKE_PROGRAM="$B2QT_PREFIX_PATH/usr/bin/ninja"
B2QT_CMAKE_BIN="$B2QT_PREFIX_PATH/usr/bin/cmake"
B2QT_TOOLCHAIN_PATH="$B2QT_PREFIX_PATH/usr/lib/cmake/Qt6/qt.toolchain.cmake"
B2QT_CXX_COMPILER_PATH="$B2QT_PREFIX_PATH/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++"
B2QT_C_COMPILER_PATH="$B2QT_PREFIX_PATH/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc"
B2QT_BUILD_DIR="$BUILD_DIR/build-b2qt-Qt-$QT_VERSION"

ensure_dir "$B2QT_BUILD_DIR"

run_cmd "$B2QT_CMAKE_BIN" --log-level=NOTICE -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$B2QT_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$B2QT_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$B2QT_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$B2QT_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$B2QT_C_COMPILER_PATH" \
    $EXTRA_CMAKE_VARIABLES

run_cmd "$B2QT_CMAKE_BIN" --build "$B2QT_BUILD_DIR" --target all

end_timer
