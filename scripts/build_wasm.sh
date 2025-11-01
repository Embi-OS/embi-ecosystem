#!/bin/bash
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer "WASM"

# Source emsdk environment if present (useful for emcc/em++)
if [ -f "$EMSCRIPTEN_PATH/emsdk_env.sh" ]; then
    # shellcheck disable=SC1091
    log "Setting up EMSDK environment"
    export EMSDK_QUIET=1
    source "$EMSCRIPTEN_PATH/emsdk_env.sh"
fi

QT_WASM_KIT="wasm_singlethread"
WASM_PREFIX_PATH="$QT_ROOT/$QT_VERSION/$QT_WASM_KIT"
WASM_MAKE_PROGRAM="$QT_ROOT/Tools/Ninja/ninja"
WASM_CMAKE_BIN="$QT_ROOT/Tools/CMake/bin/cmake"
WASM_TOOLCHAIN_PATH="$WASM_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"
WASM_CXX_COMPILER_PATH="$EMSCRIPTEN_PATH/upstream/emscripten/em++"
WASM_C_COMPILER_PATH="$EMSCRIPTEN_PATH/upstream/emscripten/emcc"
WASM_BUILD_DIR="$BUILD_DIR/build-wasm-Qt-$QT_VERSION"
WASM_OUTPUT_DIR="$BUILD_DIR/${PROJECT_NAME}_WASM/v$PROJECT_VERSION"

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$WASM_BUILD_DIR"
    rm -rf "$WASM_OUTPUT_DIR"
fi
ensure_dir "$WASM_BUILD_DIR"

run_cmd "$WASM_CMAKE_BIN" --log-level=NOTICE -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$WASM_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$WASM_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$WASM_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$WASM_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$WASM_C_COMPILER_PATH" \
    -DQT_CHAINLOAD_TOOLCHAIN_FILE="$EMSCRIPTEN_PATH/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
    $EXTRA_CMAKE_VARIABLES

run_cmd "$WASM_CMAKE_BIN" --build "$WASM_BUILD_DIR" --target all

ARTIFACT_PATTERNS=(
    "$WASM_OUTPUT_DIR/*"
)
log "WASM build completed. Verifying expected artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Packaging WASM artifacts into zip archives..."
WASM_ARTIFACT_DIR=$(prepare_artifact_dir "wasm")
if [ -d "$WASM_OUTPUT_DIR" ]; then
    shopt -s nullglob
    has_entries=0
    for item in "$WASM_OUTPUT_DIR"/*; do
        has_entries=1
        if [ -d "$item" ]; then
            app_name=$(basename "$item")
            zip_path="$WASM_ARTIFACT_DIR/A${app_name}_${SANITIZED_PROJECT_VERSION}_Wasm.zip"
            rm -f "$zip_path"
            log "Packaging WASM app $app_name → $zip_path"
            run_cmd bash -lc "cd \"$item\" && zip -r -q \"$zip_path\" ."
        elif [ -f "$item" ]; then
            log "Copying $(basename "$item") → $WASM_ARTIFACT_DIR"
            cp "$item" "$WASM_ARTIFACT_DIR/"
        fi
    done
    shopt -u nullglob
    if [ $has_entries -eq 0 ]; then
        warn "WASM output directory is empty: $WASM_OUTPUT_DIR"
    else
        log "Artifacts for wasm staged in: $WASM_ARTIFACT_DIR"
    fi
else
    warn "WASM output directory not found: $WASM_OUTPUT_DIR"
fi

end_timer "WASM"
