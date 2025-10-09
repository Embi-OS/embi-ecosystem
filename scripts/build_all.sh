#!/bin/sh

START_TIME=$(date +%s)
echo "============== Build started ==============="

"$(dirname "$0")/build_linux.sh"   "$@"
"$(dirname "$0")/build_b2qt.sh" "$@"
"$(dirname "$0")/build_wasm.sh"    "$@"

echo "====== All targets built successfully ======"
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))
mins=$((ELAPSED / 60))
secs=$((ELAPSED % 60))
echo "Build completed in ${mins}m ${secs}s"
