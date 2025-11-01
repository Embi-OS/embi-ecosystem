#!/bin/bash
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer "Linux"

QT_LINUX_KIT="gcc_64"
LINUX_PREFIX_PATH="$QT_ROOT/$QT_VERSION/$QT_LINUX_KIT"
LINUX_MAKE_PROGRAM="$QT_ROOT/Tools/Ninja/ninja"
LINUX_CMAKE_BIN="$QT_ROOT/Tools/CMake/bin/cmake"
LINUX_TOOLCHAIN_PATH="$LINUX_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"
LINUX_CXX_COMPILER_PATH="/bin/g++"
LINUX_C_COMPILER_PATH="/bin/gcc"
LINUX_BUILD_DIR="$BUILD_DIR/build-linux-Qt-$QT_VERSION"
LINUX_OUTPUT_DIR="$BUILD_DIR/${PROJECT_NAME}_Deb"

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$LINUX_BUILD_DIR"
    rm -rf "$LINUX_OUTPUT_DIR"
fi
ensure_dir "$LINUX_BUILD_DIR"

# Append linux-specific options
EXTRA_CMAKE_VARIABLES="${EXTRA_CMAKE_VARIABLES} -DEXEC_CPACK:BOOL=ON"

run_cmd "$LINUX_CMAKE_BIN" --log-level=NOTICE -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$LINUX_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$LINUX_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$LINUX_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$LINUX_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$LINUX_C_COMPILER_PATH" \
    $EXTRA_CMAKE_VARIABLES

run_cmd "$LINUX_CMAKE_BIN" --build "$LINUX_BUILD_DIR" --target all

ARTIFACT_PATTERNS=(
    "$LINUX_OUTPUT_DIR/*_${PROJECT_VERSION}_Linux-x86_64.deb"
)
log "Linux build completed. Verifying expected artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Consolidating Linux artifacts into shared staging area..."
collect_artifacts_by_patterns "linux" "${ARTIFACT_PATTERNS[@]}"

end_timer "Linux"
