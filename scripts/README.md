# Build Scripts

These scripts build the project for Linux, Windows, Boot2Qt, and WebAssembly without relying on `CMakePresets.json`.

Scripts
- `build_linux.sh` - Linux desktop build with the Qt `gcc_64` kit
- `build_windows.sh` - Windows desktop build with a Qt Windows kit, intended to run from Git Bash
- `build_b2qt.sh` - Boot2Qt build
- `build_wasm.sh` - WebAssembly build
- `build_all.sh` - runs Linux, Boot2Qt, and WASM builds
- `mysql_linux.sh` - builds and installs the Qt QMYSQL SQL driver for the Linux Qt kit
- `mysql_windows.sh` - builds and installs the Qt QMYSQL SQL driver for the Windows Qt kit, intended to run from Git Bash
- `common.sh` - shared parsing and helpers

Configuration
Machine-specific defaults can be stored in `build.conf`.
MySQL driver defaults can be stored in `mysql.conf`.
For Windows MySQL builds, set `MYSQL_WINDOWS_ROOT` to the MySQL Server or MariaDB Connector/C install directory when auto-detection does not find it. Both Git Bash paths and Windows paths are accepted:

```bash
MYSQL_WINDOWS_ROOT="/c/Program Files/MariaDB 11.8"
MYSQL_WINDOWS_ROOT="C:\Program Files\MariaDB 11.8"
```

Concrete examples:
```bash
./scripts/build_linux.sh --project-version 25.10.0 --project-version-suffix beta.1
./scripts/build_windows.sh --project-version 25.10.0 --project-version-suffix beta.1
./scripts/build_b2qt.sh --project-version 25.10.0 --project-version-suffix beta.1
./scripts/build_wasm.sh --project-version 25.10.0 --project-version-suffix beta.1
./scripts/build_yocto.sh --project-version 25.10.0 --project-version-suffix beta.1
./scripts/mysql_linux.sh --qt-version 6.8.7 --rebuild
./scripts/mysql_windows.sh --qt-version 6.8.7 --rebuild

./scripts/gitlab_release.sh --project-version 25.10.0
./scripts/build_windows.sh --optimization --project-version-suffix stable --project-version 25.10.0
```

Common options
- `--qt-root PATH`
- `--qt-version VERSION`
- `--project-version VERSION`
- `--project-version-suffix TEXT`
- `--project-version-codename TEXT`
- `--generator NAME`
- `--build-type TYPE`
- `--rebuild`
- Extra arguments are forwarded to the CMake configure step

Notes
- Build outputs are written under `../_AutoBuilds`
- `build_wasm.sh` sources `emsdk_env.sh` if present
- `build_yocto.sh` expects an existing kas workspace in `YOCTO_DIR` and runs `kas build "$YOCTO_KAS_FILE"`
- Installer packaging defaults to `PACKAGE_CPACK_THREADS=0` and `PACKAGE_IFW_ARCHIVE_COMPRESSION=1` for faster packaging. Override them with extra CMake arguments if needed.
- The MySQL driver scripts require Qt Sources for the exact Qt version/kit being used.
- Linux MySQL builds require MySQL/MariaDB client development files, for example `default-libmysqlclient-dev`, `mysql-devel`, or `mariadb-devel`. The script copies the resolved MySQL/MariaDB runtime `.so` into the Qt kit `lib` directory for packaging.
- Windows MySQL builds require MySQL Server development files or MariaDB Connector/C. The script copies `libmysql.dll` or `libmariadb.dll` into the Qt kit `bin` directory for QtCreator runs. Packaged applications must still deploy the DLL next to the executable or make it available in `PATH`.
