#!/bin/bash
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer "Windows"

QT_WINDOWS_KIT="mingw_64"
QT_WINDOWS_COMPILER_TOOLS_DIR="mingw1310_64"
WINDOWS_IFW_ROOT="$QT_ROOT_WINDOWS/Tools/QtInstallerFramework/$IFW_VERSION"
WINDOWS_PREFIX_PATH="$QT_ROOT_WINDOWS/$QT_VERSION/$QT_WINDOWS_KIT"
WINDOWS_MAKE_PROGRAM="$QT_ROOT_WINDOWS/Tools/Ninja/ninja.exe"
WINDOWS_CMAKE_BIN="$QT_ROOT_WINDOWS/Tools/CMake_64/bin/cmake.exe"
WINDOWS_TOOLCHAIN_PATH="$WINDOWS_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"
WINDOWS_CXX_COMPILER_PATH="$QT_ROOT_WINDOWS/Tools/$QT_WINDOWS_COMPILER_TOOLS_DIR/bin/g++.exe"
WINDOWS_C_COMPILER_PATH="$QT_ROOT_WINDOWS/Tools/$QT_WINDOWS_COMPILER_TOOLS_DIR/bin/gcc.exe"
WINDOWS_BUILD_DIR="$BUILD_DIR/build-windows-Qt-$QT_VERSION"
WINDOWS_PACKAGES_DIR="$BUILD_DIR/${PROJECT_NAME}_Packages"
WINDOWS_PORTABLE_DIR="$BUILD_DIR/${PROJECT_NAME}_Portable"

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$WINDOWS_BUILD_DIR"
    rm -rf "$WINDOWS_PACKAGES_DIR"
    rm -rf "$WINDOWS_PORTABLE_DIR"
fi
ensure_dir "$WINDOWS_BUILD_DIR"

# Append windows-specific options
EXTRA_CMAKE_VARIABLES+=(
    "-DEXEC_CPACK:BOOL=ON"
    "-DQTIFWDIR:PATH=${WINDOWS_IFW_ROOT}"
)

run_cmd "$WINDOWS_CMAKE_BIN" -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$WINDOWS_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$WINDOWS_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$WINDOWS_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$WINDOWS_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$WINDOWS_C_COMPILER_PATH" \
    "${EXTRA_CMAKE_VARIABLES[@]}"

run_cmd "$WINDOWS_CMAKE_BIN" --build "$WINDOWS_BUILD_DIR" --target all

ARTIFACT_PATTERNS=(
    "$WINDOWS_PACKAGES_DIR/*.exe"
    "$WINDOWS_PORTABLE_DIR/*.zip"
)
log "Windows build completed. Verifying expected artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Consolidating Windows artifacts into shared staging area..."
collect_artifacts_by_patterns "windows" "${ARTIFACT_PATTERNS[@]}"

end_timer "Windows"
