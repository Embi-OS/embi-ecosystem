#!/bin/bash
set -e

if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    cat <<'EOF'
Build and install the Qt QMYSQL SQL driver for the Linux Qt kit.

Manual prerequisites:
  - Qt kit installed with matching Qt Sources
  - Qt Maintenance Tool CMake and Ninja components
  - MySQL/MariaDB client development package
    Debian/Ubuntu: sudo apt install default-libmysqlclient-dev
    Fedora/RHEL:   sudo dnf install mysql-devel
    Arch:          sudo pacman -S mariadb-libs

Configuration:
  Edit scripts/mysql.conf or pass common options such as:
    --qt-root /opt/Qt --qt-version 6.8.7 --rebuild

Optional CMake overrides can be appended, for example:
  ./scripts/mysql_linux.sh -DMySQL_ROOT=/usr/local/mysql

Deploy note:
  The script copies the MySQL/MariaDB runtime library into the Qt kit lib
  directory for packaging. Deployed applications still need the library in
  their bundled lib directory or in the system dynamic linker path.
EOF
    exit 0
fi

. "$(dirname "$0")/common.sh"

MYSQL_CONFIG_FILE="$SCRIPT_DIR/mysql.conf"
if [ -f "$MYSQL_CONFIG_FILE" ]; then
    # shellcheck disable=SC1090
    . "$MYSQL_CONFIG_FILE"
fi

start_timer "MySQL Linux driver"

MYSQL_LINUX_QT_KIT="${MYSQL_LINUX_QT_KIT:-gcc_64}"
LINUX_PREFIX_PATH="$QT_ROOT/$QT_VERSION/$MYSQL_LINUX_QT_KIT"
LINUX_SOURCE_SQLDRIVERS_DIR="$QT_ROOT/$QT_VERSION/Src/qtbase/src/plugins/sqldrivers"
LINUX_BUILD_DIR="$BUILD_DIR/build-mysql-linux-Qt-$QT_VERSION-$MYSQL_LINUX_QT_KIT"
LINUX_CMAKE_BIN="$QT_ROOT/Tools/CMake/bin/cmake"
LINUX_MAKE_PROGRAM="$QT_ROOT/Tools/Ninja/ninja"
LINUX_TOOLCHAIN_PATH="$LINUX_PREFIX_PATH/lib/cmake/Qt6/qt.toolchain.cmake"

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

resolve_linux_mysql_include_dir() {
    first_existing_path \
        "$MYSQL_LINUX_INCLUDE_DIR" \
        "$MYSQL_LINUX_ROOT/include" \
        "$MYSQL_LINUX_ROOT/include/mysql" \
        "$MYSQL_LINUX_ROOT/include/mariadb" \
        "/usr/include/mysql" \
        "/usr/include/mariadb" \
        "/usr/local/include/mysql" \
        "/usr/local/include/mariadb"
}

resolve_linux_mysql_library() {
    first_existing_path \
        "$MYSQL_LINUX_LIBRARY" \
        "$MYSQL_LINUX_ROOT/lib/libmysqlclient.so" \
        "$MYSQL_LINUX_ROOT/lib64/libmysqlclient.so" \
        "$MYSQL_LINUX_ROOT/lib/libmariadb.so" \
        "$MYSQL_LINUX_ROOT/lib64/libmariadb.so" \
        /usr/lib/*/libmysqlclient.so \
        /usr/lib/*/libmariadb.so \
        "/usr/local/lib/libmysqlclient.so" \
        "/usr/local/lib64/libmysqlclient.so" \
        "/usr/local/lib/libmariadb.so" \
        "/usr/local/lib64/libmariadb.so"
}

resolve_real_path() {
    readlink -f "$1" 2>/dev/null || realpath "$1" 2>/dev/null || printf '%s\n' "$1"
}

resolve_linux_library_soname() {
    local library="$1"
    local soname=""

    if command -v readelf >/dev/null 2>&1; then
        soname="$(readelf -d "$library" 2>/dev/null | sed -n 's/.*Library soname: \[\(.*\)\].*/\1/p' | head -n 1)"
    fi
    if [ -z "$soname" ] && command -v objdump >/dev/null 2>&1; then
        soname="$(objdump -p "$library" 2>/dev/null | awk '/SONAME/ { print $2; exit }')"
    fi

    printf '%s\n' "$soname"
}

copy_linux_mysql_runtime_as() {
    local source="$1"
    local target_name="$2"
    local target_path="$LINUX_PREFIX_PATH/lib/$target_name"

    [ -n "$target_name" ] || return 0
    cp -f "$source" "$target_path"
    log "MySQL runtime copied to: $target_path"
}

deploy_linux_mysql_runtime() {
    local runtime_library
    local runtime_name
    local link_name
    local soname

    runtime_library="$(resolve_real_path "$MYSQL_LIBRARY")"
    [ -f "$runtime_library" ] || runtime_library="$MYSQL_LIBRARY"
    [ -f "$runtime_library" ] || return 0

    runtime_name="$(basename "$runtime_library")"
    link_name="$(basename "$MYSQL_LIBRARY")"
    soname="$(resolve_linux_library_soname "$runtime_library")"

    copy_linux_mysql_runtime_as "$runtime_library" "$runtime_name"
    if [ -n "$soname" ] && [ "$soname" != "$runtime_name" ]; then
        copy_linux_mysql_runtime_as "$runtime_library" "$soname"
    fi
    if [ -n "$link_name" ] && [ "$link_name" != "$runtime_name" ] && [ "$link_name" != "$soname" ]; then
        copy_linux_mysql_runtime_as "$runtime_library" "$link_name"
    fi
}

[ -d "$LINUX_PREFIX_PATH" ] || error_exit "Qt Linux kit not found: $LINUX_PREFIX_PATH"
[ -d "$LINUX_SOURCE_SQLDRIVERS_DIR" ] || error_exit "Qt Sources are required. Missing: $LINUX_SOURCE_SQLDRIVERS_DIR"
[ -x "$LINUX_CMAKE_BIN" ] || error_exit "Qt CMake not found or not executable: $LINUX_CMAKE_BIN"
[ -x "$LINUX_MAKE_PROGRAM" ] || error_exit "Qt Ninja not found or not executable: $LINUX_MAKE_PROGRAM"
[ -f "$LINUX_TOOLCHAIN_PATH" ] || error_exit "Qt toolchain file not found: $LINUX_TOOLCHAIN_PATH"

MYSQL_INCLUDE_DIR="$(resolve_linux_mysql_include_dir || true)"
MYSQL_LIBRARY="$(resolve_linux_mysql_library || true)"

[ -n "$MYSQL_INCLUDE_DIR" ] || error_exit "MySQL/MariaDB include directory not found. Install development files or set MYSQL_LINUX_INCLUDE_DIR in scripts/mysql.conf."
[ -f "$MYSQL_INCLUDE_DIR/mysql.h" ] || error_exit "mysql.h not found in: $MYSQL_INCLUDE_DIR"
[ -n "$MYSQL_LIBRARY" ] || error_exit "MySQL/MariaDB client library not found. Install development files or set MYSQL_LINUX_LIBRARY in scripts/mysql.conf."

if [ "$REBUILD" -eq 1 ]; then
    rm -rf "$LINUX_BUILD_DIR"
fi
ensure_dir "$LINUX_BUILD_DIR"

log "Qt kit: $LINUX_PREFIX_PATH"
log "Qt SQL driver sources: $LINUX_SOURCE_SQLDRIVERS_DIR"
log "MySQL include: $MYSQL_INCLUDE_DIR"
log "MySQL library: $MYSQL_LIBRARY"

run_cmd "$LINUX_CMAKE_BIN" -G "$GENERATOR" -S "$LINUX_SOURCE_SQLDRIVERS_DIR" -B "$LINUX_BUILD_DIR" \
    -DCMAKE_MAKE_PROGRAM:FILEPATH="$LINUX_MAKE_PROGRAM" \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH="$LINUX_TOOLCHAIN_PATH" \
    -DCMAKE_BUILD_TYPE:STRING="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX:PATH="$LINUX_PREFIX_PATH" \
    -DMySQL_INCLUDE_DIR:PATH="$MYSQL_INCLUDE_DIR" \
    -DMySQL_LIBRARY:FILEPATH="$MYSQL_LIBRARY" \
    -DQT_GENERATE_SBOM:BOOL=OFF \
    "${EXTRA_ARGS[@]}"

run_cmd "$LINUX_CMAKE_BIN" --build "$LINUX_BUILD_DIR" --target all
[ -w "$LINUX_PREFIX_PATH" ] || error_exit "Qt kit is not writable: $LINUX_PREFIX_PATH. Install the built driver with: sudo \"$LINUX_CMAKE_BIN\" --install \"$LINUX_BUILD_DIR\""
run_cmd "$LINUX_CMAKE_BIN" --install "$LINUX_BUILD_DIR"
deploy_linux_mysql_runtime

log "QMYSQL driver installed into: $LINUX_PREFIX_PATH/plugins/sqldrivers"
end_timer "MySQL Linux driver"
