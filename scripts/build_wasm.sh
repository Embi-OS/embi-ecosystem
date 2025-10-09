#!/bin/sh
set -e

# Load common config and helpers
. "$(dirname "$0")/common.sh"

start_timer

EMSCRIPTEN_PATH="/home/romain-dev/A_VOH/emsdk"
QT_WASM_KIT="wasm_singlethread"
WASM_PREFIX_PATH="$QT_ROOT/$QT_VERSION/$QT_WASM_KIT"
WASM_MAKE_PROGRAM="$QT_ROOT/Tools/Ninja/ninja"
WASM_CMAKE_BIN="$QT_ROOT/Tools/CMake/bin/cmake"
WASM_TOOLCHAIN_PATH="$WASM_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"
WASM_CXX_COMPILER_PATH="$EMSCRIPTEN_PATH/upstream/emscripten/em++"
WASM_C_COMPILER_PATH="$EMSCRIPTEN_PATH/upstream/emscripten/emcc"
WASM_BUILD_DIR="$BUILD_DIR/build-wasm-Qt-$QT_VERSION"

ensure_dir "$WASM_BUILD_DIR"

run_cmd "$WASM_CMAKE_BIN" --log-level=NOTICE -G "$GENERATOR" -S "$PROJECT_ROOT" -B "$WASM_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$WASM_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$WASM_TOOLCHAIN_PATH" \
    -DCMAKE_CXX_COMPILER:FILEPATH="$WASM_CXX_COMPILER_PATH" \
    -DCMAKE_C_COMPILER:FILEPATH="$WASM_C_COMPILER_PATH" \
    -DQT_CHAINLOAD_TOOLCHAIN_FILE="$EMSCRIPTEN_PATH/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
    $EXTRA_CMAKE_VARIABLES

run_cmd "$WASM_CMAKE_BIN" --build "$WASM_BUILD_DIR" --target all

end_timer
