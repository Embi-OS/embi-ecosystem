#!/bin/bash
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer "Boot2Qt"

B2QT_PREFIX_PATH="$QT_ROOT/$QT_VERSION/Boot2Qt/$B2QT_KIT/toolchain/sysroots/x86_64-pokysdk-linux"
B2QT_MAKE_PROGRAM="$B2QT_PREFIX_PATH/usr/bin/ninja"
B2QT_CMAKE_BIN="$B2QT_PREFIX_PATH/usr/bin/cmake"
B2QT_TOOLCHAIN_PATH="$B2QT_PREFIX_PATH/usr/lib/cmake/Qt6/qt.toolchain.cmake"
B2QT_CXX_COMPILER_PATH="$B2QT_PREFIX_PATH/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++"
B2QT_C_COMPILER_PATH="$B2QT_PREFIX_PATH/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc"
B2QT_BUILD_DIR="$BUILD_DIR/build-b2qt-Qt-$QT_VERSION"
B2QT_OUTPUT_DIR="$BUILD_DIR/${PROJECT_NAME}_Boot2Qt_$QT_VERSION/v$PROJECT_VERSION"

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$B2QT_BUILD_DIR"
    rm -rf "$B2QT_OUTPUT_DIR"
fi
ensure_dir "$B2QT_BUILD_DIR"

run_cmd "$B2QT_CMAKE_BIN" --log-level=NOTICE -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$B2QT_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$B2QT_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$B2QT_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$B2QT_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$B2QT_C_COMPILER_PATH" \
    $EXTRA_CMAKE_VARIABLES

run_cmd "$B2QT_CMAKE_BIN" --build "$B2QT_BUILD_DIR" --target all

ARTIFACT_PATTERNS=(
    "$B2QT_OUTPUT_DIR/*"
)
log "BootQt build completed. Verifying expected artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Packaging Boot2Qt artifacts into a single archive..."
B2QT_ARTIFACT_DIR=$(prepare_artifact_dir "boot2qt")
if [ -d "$B2QT_OUTPUT_DIR" ]; then
    if [ -z "$(ls -A "$B2QT_OUTPUT_DIR")" ]; then
        warn "Boot2Qt output directory is empty: $B2QT_OUTPUT_DIR"
    else
        ZIP_BASENAME="${SANITIZED_PROJECT_NAME}_${SANITIZED_PROJECT_VERSION}_Boot2Qt.zip"
        ZIP_PATH="$B2QT_ARTIFACT_DIR/$ZIP_BASENAME"
        log "Compressing Boot2Qt outputs → $ZIP_PATH"
        rm -f "$ZIP_PATH"
        run_cmd bash -lc "cd \"$B2QT_OUTPUT_DIR\" && zip -r -q \"$ZIP_PATH\" ."
        log "Artifacts for boot2qt staged in: $B2QT_ARTIFACT_DIR"
    fi
else
    warn "Boot2Qt output directory not found: $B2QT_OUTPUT_DIR"
fi

end_timer "Boot2Qt"
