#!/bin/bash
set -e

if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    cat <<'EOF'
Build and install the Qt QMYSQL SQL driver for the Windows Qt kit.

Run from Git Bash on Windows.

Manual prerequisites:
  - Qt Windows kit installed with matching Qt Sources
  - Qt Maintenance Tool CMake and Ninja components
  - Matching compiler toolchain for the Qt kit
  - MySQL Server full install or MariaDB Connector/C with:
      include/mysql.h
      lib/libmysql.lib or lib/libmariadb.lib

Configuration:
  Edit scripts/mysql.conf or pass common options such as:
    --qt-root /c/Qt --qt-version 6.8.7 --rebuild
  MYSQL_WINDOWS_ROOT, MYSQL_WINDOWS_INCLUDE_DIR, and MYSQL_WINDOWS_LIBRARY
  may use Git Bash paths or Windows paths.

Deploy note:
  The script copies libmysql.dll or libmariadb.dll into the Qt kit bin
  directory for QtCreator runs. Deployed applications still need the DLL next
  to the executable or in PATH.
EOF
    exit 0
fi

USER_QT_ROOT=""
NEXT_IS_QT_ROOT=0
for arg in "$@"; do
    if [ "$NEXT_IS_QT_ROOT" -eq 1 ]; then
        USER_QT_ROOT="$arg"
        NEXT_IS_QT_ROOT=0
        continue
    fi

    if [ "$arg" = "--qt-root" ]; then
        NEXT_IS_QT_ROOT=1
    fi
done

. "$(dirname "$0")/common.sh"

MYSQL_CONFIG_FILE="$SCRIPT_DIR/mysql.conf"
if [ -f "$MYSQL_CONFIG_FILE" ]; then
    # shellcheck disable=SC1090
    . "$MYSQL_CONFIG_FILE"
fi

start_timer "MySQL Windows driver"

if [ -n "$USER_QT_ROOT" ]; then
    QT_ROOT_WINDOWS="$USER_QT_ROOT"
else
    QT_ROOT_WINDOWS="${QT_ROOT_WINDOWS:-/c/Qt}"
fi
MYSQL_WINDOWS_QT_KIT="${MYSQL_WINDOWS_QT_KIT:-mingw_64}"
MYSQL_WINDOWS_COMPILER_TOOLS_DIR="${MYSQL_WINDOWS_COMPILER_TOOLS_DIR:-mingw1310_64}"
MYSQL_CMAKE_CONFIGURATION_TYPES="${MYSQL_CMAKE_CONFIGURATION_TYPES:-Release;Debug}"

WINDOWS_PREFIX_PATH="$QT_ROOT_WINDOWS/$QT_VERSION/$MYSQL_WINDOWS_QT_KIT"
WINDOWS_SOURCE_SQLDRIVERS_DIR="$QT_ROOT_WINDOWS/$QT_VERSION/Src/qtbase/src/plugins/sqldrivers"
WINDOWS_BUILD_DIR="$BUILD_DIR/build-mysql-windows-Qt-$QT_VERSION-$MYSQL_WINDOWS_QT_KIT"
WINDOWS_CMAKE_BIN="$QT_ROOT_WINDOWS/Tools/CMake_64/bin/cmake.exe"
WINDOWS_MAKE_PROGRAM="$QT_ROOT_WINDOWS/Tools/Ninja/ninja.exe"
WINDOWS_TOOLCHAIN_PATH="$WINDOWS_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"
WINDOWS_CXX_COMPILER_PATH="$QT_ROOT_WINDOWS/Tools/$MYSQL_WINDOWS_COMPILER_TOOLS_DIR/bin/g++.exe"
WINDOWS_C_COMPILER_PATH="$QT_ROOT_WINDOWS/Tools/$MYSQL_WINDOWS_COMPILER_TOOLS_DIR/bin/gcc.exe"
WINDOWS_RC_COMPILER_PATH="$QT_ROOT_WINDOWS/Tools/$MYSQL_WINDOWS_COMPILER_TOOLS_DIR/bin/windres.exe"

first_existing_path() {
    local candidate
    for candidate in "$@"; do
        if [ -n "$candidate" ] && [ -e "$candidate" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

first_existing_dir_with_file() {
    local required_file="$1"
    shift

    local candidate
    for candidate in "$@"; do
        if [ -n "$candidate" ] && [ -f "$candidate/$required_file" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

to_git_bash_path() {
    local path="$1"
    [ -n "$path" ] || return 0

    if command -v cygpath >/dev/null 2>&1; then
        cygpath -u "$path" 2>/dev/null && return 0
    fi

    if [[ "$path" =~ ^([A-Za-z]):[\\/](.*)$ ]]; then
        local drive
        local rest
        drive="$(printf '%s' "${BASH_REMATCH[1]}" | tr '[:upper:]' '[:lower:]')"
        rest="${BASH_REMATCH[2]//\\//}"
        printf '/%s/%s\n' "$drive" "$rest"
        return 0
    fi

    printf '%s\n' "$path"
}

to_cmake_path() {
    local path="$1"
    [ -n "$path" ] || return 0

    if command -v cygpath >/dev/null 2>&1; then
        if [[ "$path" == *" "* ]]; then
            cygpath -m -s "$path" 2>/dev/null && return 0
        fi

        cygpath -m "$path" 2>/dev/null && return 0
    fi

    printf '%s\n' "$path"
}

WINDOWS_MYSQL_ROOT_CANDIDATES=()

add_windows_mysql_root_candidate() {
    local candidate
    candidate="$(to_git_bash_path "$1")"
    [ -n "$candidate" ] || return 0
    WINDOWS_MYSQL_ROOT_CANDIDATES+=("$candidate")
}

add_windows_mysql_root_glob_candidates() {
    local pattern="$1"
    local match
    while IFS= read -r match; do
        add_windows_mysql_root_candidate "$match"
    done < <(compgen -G "$pattern" || true)
}

collect_windows_mysql_root_candidates() {
    add_windows_mysql_root_candidate "$MYSQL_WINDOWS_ROOT"
    add_windows_mysql_root_candidate "$MYSQL_ROOT"
    add_windows_mysql_root_candidate "$MARIADB_ROOT"

    add_windows_mysql_root_glob_candidates "/c/Program Files/MariaDB *"
    add_windows_mysql_root_glob_candidates "/c/Program Files/MariaDB/MariaDB Connector C*"
    add_windows_mysql_root_glob_candidates "/c/Program Files/MariaDB/MariaDB Server *"
    add_windows_mysql_root_glob_candidates "/c/Program Files/MySQL/MySQL Server *"
    add_windows_mysql_root_glob_candidates "/c/Program Files/MySQL/MySQL Connector C *"
}

resolve_windows_mysql_include_dir() {
    local root
    local candidates=()

    candidates+=("$(to_git_bash_path "$MYSQL_WINDOWS_INCLUDE_DIR")")
    for root in "${WINDOWS_MYSQL_ROOT_CANDIDATES[@]}"; do
        candidates+=(
            "$root/include"
            "$root/include/mysql"
            "$root/include/mariadb"
        )
    done

    first_existing_dir_with_file "mysql.h" "${candidates[@]}"
}

resolve_windows_mysql_library() {
    local root
    local candidates=()

    candidates+=("$(to_git_bash_path "$MYSQL_WINDOWS_LIBRARY")")
    for root in "${WINDOWS_MYSQL_ROOT_CANDIDATES[@]}"; do
        candidates+=(
            "$root/lib/libmysql.lib"
            "$root/lib/libmariadb.lib"
            "$root/lib/mariadb/libmariadb.lib"
            "$root/lib/libmysql.dll.a"
            "$root/lib/libmariadb.dll.a"
            "$root/lib/mariadb/libmariadb.dll.a"
        )
    done

    first_existing_path "${candidates[@]}"
}

resolve_windows_mysql_runtime() {
    local root
    local candidates=()

    for root in "${WINDOWS_MYSQL_ROOT_CANDIDATES[@]}"; do
        candidates+=(
            "$root/lib/libmysql.dll"
            "$root/lib/libmariadb.dll"
            "$root/lib/mariadb/libmariadb.dll"
            "$root/bin/libmysql.dll"
            "$root/bin/libmariadb.dll"
        )
    done

    first_existing_path "${candidates[@]}"
}

stage_windows_mysql_client_files() {
    local stage_dir="$WINDOWS_BUILD_DIR/mysql-client"
    local include_dir="$stage_dir/include"
    local library_path="$stage_dir/$(basename "$MYSQL_LIBRARY")"

    ensure_dir "$include_dir"
    cp -R "$MYSQL_INCLUDE_DIR"/. "$include_dir"/
    cp "$MYSQL_LIBRARY" "$library_path"

    MYSQL_CMAKE_INCLUDE_DIR="$(to_cmake_path "$include_dir")"
    MYSQL_CMAKE_LIBRARY="$(to_cmake_path "$library_path")"
}

deploy_windows_mysql_runtime() {
    [ -n "$MYSQL_RUNTIME_LIBRARY" ] || return 0

    local runtime_target_dir="$WINDOWS_PREFIX_PATH/bin"
    local runtime_target="$runtime_target_dir/$(basename "$MYSQL_RUNTIME_LIBRARY")"

    ensure_dir "$runtime_target_dir"
    cp "$MYSQL_RUNTIME_LIBRARY" "$runtime_target"
    log "MySQL runtime copied to: $runtime_target"
}

print_windows_mysql_search_hint() {
    warn "Set MYSQL_WINDOWS_ROOT in scripts/mysql.conf to the MySQL Server or MariaDB Connector/C install directory."
    warn "Git Bash example: MYSQL_WINDOWS_ROOT=\"/c/Program Files/MariaDB 11.8\""
    warn "Windows path example: MYSQL_WINDOWS_ROOT=\"C:\\Program Files\\MariaDB 11.8\""

    if [ "${#WINDOWS_MYSQL_ROOT_CANDIDATES[@]}" -gt 0 ]; then
        warn "Searched MySQL/MariaDB roots:"
        local root
        for root in "${WINDOWS_MYSQL_ROOT_CANDIDATES[@]}"; do
            warn "  $root"
        done
    fi
}

[ -d "$WINDOWS_PREFIX_PATH" ] || error_exit "Qt Windows kit not found: $WINDOWS_PREFIX_PATH"
[ -d "$WINDOWS_SOURCE_SQLDRIVERS_DIR" ] || error_exit "Qt Sources are required. Missing: $WINDOWS_SOURCE_SQLDRIVERS_DIR"
[ -f "$WINDOWS_CMAKE_BIN" ] || error_exit "Qt CMake not found: $WINDOWS_CMAKE_BIN"
[ -f "$WINDOWS_MAKE_PROGRAM" ] || error_exit "Qt Ninja not found: $WINDOWS_MAKE_PROGRAM"
[ -f "$WINDOWS_TOOLCHAIN_PATH" ] || error_exit "Qt toolchain file not found: $WINDOWS_TOOLCHAIN_PATH"

if [[ "$MYSQL_WINDOWS_QT_KIT" == mingw* ]]; then
    [ -f "$WINDOWS_CXX_COMPILER_PATH" ] || error_exit "MinGW C++ compiler not found: $WINDOWS_CXX_COMPILER_PATH"
    [ -f "$WINDOWS_C_COMPILER_PATH" ] || error_exit "MinGW C compiler not found: $WINDOWS_C_COMPILER_PATH"
    [ -f "$WINDOWS_RC_COMPILER_PATH" ] || error_exit "MinGW resource compiler not found: $WINDOWS_RC_COMPILER_PATH"
fi

collect_windows_mysql_root_candidates

MYSQL_INCLUDE_DIR="$(resolve_windows_mysql_include_dir || true)"
MYSQL_LIBRARY="$(resolve_windows_mysql_library || true)"
MYSQL_RUNTIME_LIBRARY="$(resolve_windows_mysql_runtime || true)"

if [ -z "$MYSQL_INCLUDE_DIR" ]; then
    print_windows_mysql_search_hint
    error_exit "mysql.h not found. Install MySQL Server development files or MariaDB Connector/C, then set MYSQL_WINDOWS_ROOT or MYSQL_WINDOWS_INCLUDE_DIR."
fi

if [ -z "$MYSQL_LIBRARY" ]; then
    print_windows_mysql_search_hint
    error_exit "MySQL/MariaDB import library not found. Set MYSQL_WINDOWS_ROOT or MYSQL_WINDOWS_LIBRARY."
fi

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$WINDOWS_BUILD_DIR"
fi
ensure_dir "$WINDOWS_BUILD_DIR"
stage_windows_mysql_client_files

log "Qt kit: $WINDOWS_PREFIX_PATH"
log "Qt SQL driver sources: $WINDOWS_SOURCE_SQLDRIVERS_DIR"
log "MySQL include: $MYSQL_INCLUDE_DIR"
log "MySQL library: $MYSQL_LIBRARY"
log "MySQL CMake include: $MYSQL_CMAKE_INCLUDE_DIR"
log "MySQL CMake library: $MYSQL_CMAKE_LIBRARY"
if [ -n "$MYSQL_RUNTIME_LIBRARY" ]; then
    log "MySQL runtime library: $MYSQL_RUNTIME_LIBRARY"
else
    warn "Runtime DLL not found. Copy libmysql.dll or libmariadb.dll next to the application executable or add it to PATH."
fi

CMAKE_COMPILER_ARGS=()
if [[ "$MYSQL_WINDOWS_QT_KIT" == mingw* ]]; then
    export PATH="$(dirname "$WINDOWS_CXX_COMPILER_PATH"):$PATH"
    CMAKE_COMPILER_ARGS=(
        -DCMAKE_CXX_COMPILER:FILEPATH="$WINDOWS_CXX_COMPILER_PATH"
        -DCMAKE_C_COMPILER:FILEPATH="$WINDOWS_C_COMPILER_PATH"
        -DCMAKE_RC_COMPILER:FILEPATH="$WINDOWS_RC_COMPILER_PATH"
    )
fi

run_cmd "$WINDOWS_CMAKE_BIN" -G "Ninja Multi-Config" -S "$WINDOWS_SOURCE_SQLDRIVERS_DIR" -B "$WINDOWS_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$WINDOWS_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$WINDOWS_TOOLCHAIN_PATH" \
    -DCMAKE_INSTALL_PREFIX:PATH="$WINDOWS_PREFIX_PATH" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="$MYSQL_CMAKE_CONFIGURATION_TYPES" \
    -DMySQL_INCLUDE_DIR:PATH="$MYSQL_CMAKE_INCLUDE_DIR" \
    -DMySQL_LIBRARY:FILEPATH="$MYSQL_CMAKE_LIBRARY" \
    -DQT_GENERATE_SBOM:BOOL=OFF \
    "${CMAKE_COMPILER_ARGS[@]}" \
    "${EXTRA_ARGS[@]}"

IFS=';' read -r -a MYSQL_WINDOWS_CONFIGS <<< "$MYSQL_CMAKE_CONFIGURATION_TYPES"
for config in "${MYSQL_WINDOWS_CONFIGS[@]}"; do
    [ -n "$config" ] || continue
    run_cmd "$WINDOWS_CMAKE_BIN" --build "$WINDOWS_BUILD_DIR" --target all --config "$config"
    [ -w "$WINDOWS_PREFIX_PATH" ] || error_exit "Qt kit is not writable: $WINDOWS_PREFIX_PATH. Run Git Bash as administrator or install Qt in a user-writable location."
    run_cmd "$WINDOWS_CMAKE_BIN" --install "$WINDOWS_BUILD_DIR" --config "$config"
done

deploy_windows_mysql_runtime

log "QMYSQL driver installed into: $WINDOWS_PREFIX_PATH/plugins/sqldrivers"
warn "Remember to deploy libmysql.dll or libmariadb.dll next to packaged application executables."
end_timer "MySQL Windows driver"
