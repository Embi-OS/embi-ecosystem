#!/bin/bash
set -e

log() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

START_TIME=$(date +%s)
log "============== Build started ==============="

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

"$SCRIPT_DIR/build_linux.sh" "$@"
"$SCRIPT_DIR/build_b2qt.sh" "$@"
"$SCRIPT_DIR/build_wasm.sh" "$@"
"$SCRIPT_DIR/build_yocto.sh" "$@"

log "====== All targets built successfully ======"
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))
mins=$((ELAPSED / 60))
secs=$((ELAPSED % 60))
log "Build completed in ${mins}m ${secs}s"
