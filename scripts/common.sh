#!/bin/sh
# Common environment and helper functions for Qt project builds

# Root project directory (relative to this script's location)
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
PROJECT_PARENT="$(cd "$PROJECT_ROOT/.." && pwd)"
BUILD_DIR="$PROJECT_PARENT/_AutoBuilds"

JOBS=$(( $(nproc) ))

# Default Qt installation root
QT_ROOT="/opt/Qt"

# Default CMake generator
GENERATOR="Ninja"

# Default build type
BUILD_TYPE="Release"

# Default Qt and project versions
REBUILD=0
QT_VERSION="6.8.4"
PROJECT_VERSION="1.0.0"

# Parse command-line arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --rebuild)
            REBUILD=1
            ;;
        --qt-version)
            QT_VERSION="$2"
            shift
            ;;
        --project-version)
            PROJECT_VERSION="$2"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--rebuild] [--qt-version <version>] [--project-version <version>]"
            exit 1
            ;;
    esac
    shift
done

# Define common CMake variables passed to all builds
EXTRA_CMAKE_VARIABLES="-DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
-DCMAKE_CXX_FLAGS="-w" -DCMAKE_C_FLAGS="-w" \
-DCMAKE_SUPPRESS_DEVELOPER_WARNINGS:BOOL=ON \
-DDEFAULT_PROJECT_VERSION=${PROJECT_VERSION} \
-DENABLE_OPTIMIZATION:BOOL=ON \
-DEXEC_CPACK:BOOL=ON"

# -------- Helpers --------
run_cmd() {
    echo ">>> Running: $*"
    "$@"
    if [ $? -ne 0 ]; then
        echo "!!! Command failed: $@"
        exit 1
    fi
}

ensure_dir() {
    if [ "$REBUILD" -eq 1 ]; then
        rm -rf "$1"
    fi
    
    [ -d "$1" ] || mkdir -p "$1"
}

# -------- Timing helpers --------
start_timer() {
    echo "=== Script started ==="
    START_TIME=$(date +%s)
}

end_timer() {
    END_TIME=$(date +%s)
    ELAPSED=$((END_TIME - START_TIME))
    mins=$((ELAPSED / 60))
    secs=$((ELAPSED % 60))
    echo "=== Script completed in ${mins}m ${secs}s ==="
}
