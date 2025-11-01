#!/bin/bash
# Common environment and helper functions for Qt project builds

# Determine script and project directories
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_PARENT="$(cd "$PROJECT_ROOT/.." && pwd)"
BUILD_DIR="$PROJECT_PARENT/_AutoBuilds"

JOBS=$(( $(nproc) ))

# Defaults
QT_ROOT="/opt/Qt"
GENERATOR="Ninja"
BUILD_TYPE="Release"
REBUILD=0
QT_VERSION="6.8.5"
PROJECT_NAME="Project"
PROJECT_VERSION="1.0.0"
PROJECT_DESCRIPTION="Chasseral"
PROJECT_BRANCH="dev"
EMSCRIPTEN_PATH="/opt/emsdk"

# Collect extra CMake args (forwarded to configure step)
EXTRA_ARGS=""

# Optional local config file to override defaults (place next to scripts)
CONFIG_FILE="$SCRIPT_DIR/build.conf"
if [ -f "$CONFIG_FILE" ]; then
    # shellcheck disable=SC1090
    . "$CONFIG_FILE"
fi

# Parse command-line arguments
while [ $# -gt 0 ]; do
    case "$1" in
        --rebuild)
            echo "option: $1"
            REBUILD=1
            ;;
        --qt-version)
            echo "option: $1"
            QT_VERSION="$2"
            shift
            ;;
        --qt-root)
            echo "option: $1"
            QT_ROOT="$2"
            shift
            ;;
        --project-version)
            echo "option: $1"
            PROJECT_VERSION="$2"
            shift
            ;;
        --project-description)
            echo "option: $1"
            PROJECT_DESCRIPTION="$2"
            shift
            ;;
        --emsdk-path|--emscripten-path)
            echo "option: $1"
            EMSCRIPTEN_PATH="$2"
            shift
            ;;
        --generator)
            echo "option: $1"
            GENERATOR="$2"
            shift
            ;;
        --build-type)
            echo "option: $1"
            BUILD_TYPE="$2"
            shift
            ;;
        *)
            echo "Extra option: $1"
            EXTRA_ARGS="$EXTRA_ARGS $1"
            ;;
    esac
    shift
done

SANITIZED_PROJECT_NAME=$(printf '%s' "$PROJECT_NAME" | tr '[:space:]' '_' | tr -c 'A-Za-z0-9._-' '_')
[ -n "$SANITIZED_PROJECT_NAME" ] || SANITIZED_PROJECT_NAME="project"
SANITIZED_PROJECT_VERSION=$(printf '%s' "$PROJECT_VERSION" | tr '[:space:]' '_' | tr -c 'A-Za-z0-9._-' '_')
[ -n "$SANITIZED_PROJECT_VERSION" ] || SANITIZED_PROJECT_VERSION="0.0.0"
SANITIZED_PROJECT_VERSION=$(IFS=.; for x in $SANITIZED_PROJECT_VERSION; do printf "%02d" "$x"; done)

# Define common CMake variables passed to all builds
EXTRA_CMAKE_VARIABLES="-DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
-DCMAKE_CXX_FLAGS:STRING=-w -DCMAKE_C_FLAGS:STRING=-w \
-DCMAKE_SUPPRESS_DEVELOPER_WARNINGS:BOOL=ON \
-DDEFAULT_PROJECT_VERSION:STRING=${PROJECT_VERSION} \
-DDEFAULT_PROJECT_DESCRIPTION:STRING=${PROJECT_DESCRIPTION} \
-DSANITIZED_PROJECT_VERSION:STRING=${SANITIZED_PROJECT_VERSION} \
-DENABLE_OPTIMIZATION:BOOL=ON \
-DDISABLE_SAMPLES:BOOL=ON \
-DDISABLE_TESTS:BOOL=ON"

# Append all extra/unknown arguments
EXTRA_CMAKE_VARIABLES="${EXTRA_CMAKE_VARIABLES} ${EXTRA_ARGS}"

# -------- Helpers --------
log() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

warn() {
    echo -e "\033[1;33m[WARN]\033[0m $1"
}

error_exit() {
    echo -e "\033[1;31m[ERROR]\033[0m $1"
    exit 1
}

ensure_dir() {
    [ -d "$1" ] || mkdir -p "$1"
}

run_cmd() {
    local quiet=0
    local full=0
    while [[ $# -gt 0 && "$1" == --* ]]; do
        case "$1" in
            --quiet)
                quiet=1
                ;;
            --full)
                full=1
                ;;
            --)
                shift
                break
                ;;
            *)
                error_exit "run_cmd: unknown option $1"
                ;;
        esac
        shift
    done

    if [ $quiet -eq 1 ] && [ $full -eq 1 ]; then
        error_exit "run_cmd: --quiet and --full cannot be combined"
    fi

    if [ $# -eq 0 ]; then
        error_exit "run_cmd: missing command to execute"
    fi

    if [ $quiet -eq 1 ]; then
        log ">>> Running (quiet): $*"
        local tmp
        tmp=$(mktemp -t run_cmd.XXXXXX)
        if ! "$@" >"$tmp" 2>&1; then
            while IFS= read -r line; do
                warn "    $line"
            done <"$tmp"
            rm -f "$tmp"
            error_exit "!!! Command failed: $*"
        fi
        rm -f "$tmp"
    elif [ $full -eq 1 ]; then
        log ">>> Running: $*"
        "$@"
        if [ $? -ne 0 ]; then
            error_exit "!!! Command failed: $*"
        fi
    else
        log ">>> Running: $*"
        # Show command output dynamically (overwrite same line)
        local line
        # Use unbuffered output and carriage return to update the same line
        "$@" 2>&1 | while IFS= read -r line; do
            # Trim to avoid line wrapping issues; customize as you wish
            printf "\r\033[K%s" "$line"
        done
        local exit_code=${PIPESTATUS[0]}  # preserve the real command’s exit code
        echo ""  # move to a new line after command finishes
        if [ $exit_code -ne 0 ]; then
            error_exit "!!! Command failed: $*"
        fi
    fi
}

# -------- Timing helpers --------
start_timer() {
    log "=== $1 script started ==="
    START_TIME=$(date +%s)
}

end_timer() {
    END_TIME=$(date +%s)
    ELAPSED=$((END_TIME - START_TIME))
    mins=$((ELAPSED / 60))
    secs=$((ELAPSED % 60))
    log "=== $1 script completed in ${mins}m ${secs}s ==="
}


# -------- Artifacts helpers --------
# Determine consolidated artifact directories (sanitized for filesystem safety)
ARTIFACTS_VERSION_DIR="$BUILD_DIR/artifacts_${SANITIZED_PROJECT_NAME}_${SANITIZED_PROJECT_VERSION}"

verify_artifacts() {
    local pattern
    local matches=()
    for pattern in "$@"; do
        mapfile -t matches < <(compgen -G "$pattern" || true)
        if [ "${#matches[@]}" -eq 0 ]; then
            warn "!!! Missing artifact: $pattern"
        else
            for match in "${matches[@]}"; do
                if [ -L "$match" ]; then
                    local resolved
                    resolved=$(readlink -f "$match" 2>/dev/null || realpath "$match" 2>/dev/null || true)
                    log "  ✓ Found artifact: $match"
                    log "          symlink → $resolved"
                else
                    log "  ✓ Found artifact: $match"
                fi
            done
        fi
    done
}

prepare_artifact_dir() {
    ensure_dir "$ARTIFACTS_VERSION_DIR"
    printf '%s\n' "$ARTIFACTS_VERSION_DIR"
}

collect_artifacts_by_patterns() {
    local target="$1"
    shift

    if [ $# -eq 0 ]; then
        warn "collect_artifacts_by_patterns: no patterns provided for $target"
        return
    fi

    local staging_dir
    staging_dir=$(prepare_artifact_dir "$target")

    local pattern
    local matches=()
    local match
    local copied=0

    for pattern in "$@"; do
        mapfile -t matches < <(compgen -G "$pattern" || true)
        if [ "${#matches[@]}" -eq 0 ]; then
            warn "No artifacts matched pattern: $pattern"
            continue
        fi
        for match in "${matches[@]}"; do
            local source="$match"
            local dest_name
            if [ -L "$match" ]; then
                local resolved
                resolved=$(readlink -f "$match" 2>/dev/null || realpath "$match" 2>/dev/null || "")
                if [ -z "$resolved" ]; then
                    warn "Unable to resolve symlink: $match"
                    continue
                fi
                source="$resolved"
                dest_name=$(basename "$resolved")
                log "Copying symlink target $(basename "$resolved") → $staging_dir"
            else
                dest_name=$(basename "$match")
                log "Copying $dest_name → $staging_dir"
            fi

            if [ -d "$source" ]; then
                warn "Skipping directory artifact (expected file): $source"
                continue
            fi

            local dest_path="$staging_dir/$dest_name"
            if [ -e "$dest_path" ]; then
                rm -f "$dest_path"
            fi
            cp "$source" "$dest_path"
            copied=1
        done
    done

    if [ $copied -eq 0 ]; then
        warn "collect_artifacts_by_patterns: nothing copied for $target"
        return
    fi

    log "Artifacts for $target staged in: $staging_dir"
}
