#!/bin/bash
set -e

# Yocto/kas defaults (can be overridden in build.conf)
YOCTO_DIR="$PROJECT_PARENT/_YoctoBuilds"
YOCTO_KAS_FILE="embi-kas/latest.yml"
YOCTO_MACHINE="raspberrypi-armv8"
YOCTO_IMAGE_NAME="embi-image"
YOCTO_IMAGE_EXTENSION="wic.xz"
KAS_BIN="kas"

# Load common config and helpers
. "$(dirname "$0")/common.sh"

# Expand leading ~ if present in paths
YOCTO_DIR="${YOCTO_DIR/#\~/$HOME}"
YOCTO_KAS_FILE="${YOCTO_KAS_FILE/#\~/$HOME}"

start_timer "Yocto"

require_tools "$KAS_BIN"

[ -d "$YOCTO_DIR" ] || error_exit "Yocto kas workspace not found: $YOCTO_DIR"

if [[ "$YOCTO_KAS_FILE" = /* ]]; then
    KAS_CONFIG_ARG="$YOCTO_KAS_FILE"
    KAS_CONFIG_PATH="$YOCTO_KAS_FILE"
else
    KAS_CONFIG_ARG="$YOCTO_KAS_FILE"
    KAS_CONFIG_PATH="$YOCTO_DIR/$YOCTO_KAS_FILE"
fi

[ -f "$KAS_CONFIG_PATH" ] || error_exit "kas config not found: $KAS_CONFIG_PATH"

KAS_BUILD_CMD=(
    env
    "PRODUCT_VERSION=$PROJECT_VERSION"
    "PRODUCT_VERSION_SUFFIX=$PROJECT_VERSION_SUFFIX"
    "PRODUCT_VERSION_CODENAME=$PROJECT_VERSION_CODENAME"
    "PRODUCT_IMAGE_BRANCH=$PROJECT_BRANCH"
    "$KAS_BIN" build --update "$KAS_CONFIG_ARG"
)

log "Starting Yocto build with kas (non-deterministic hash warnings will be ignored)"
log "Workspace: $YOCTO_DIR"
log "Config: $KAS_CONFIG_ARG"
log ">>> cd \"$YOCTO_DIR\" && ${KAS_BUILD_CMD[*]}"
if ! (
    cd "$YOCTO_DIR"
    "${KAS_BUILD_CMD[@]}"
); then
    warn "Yocto build command reported a non-zero exit. Proceeding anyway; inspect Yocto logs if this is unexpected."
fi

YOCTO_BUILD_DIR="${YOCTO_DIR}/build"
IMAGE_DIR="$YOCTO_BUILD_DIR/tmp/deploy/images/$YOCTO_MACHINE"
QBSP_DIR="$YOCTO_BUILD_DIR/tmp/deploy/qbsp"

# Verify that expected artifacts were produced (patterns are globs)
ARTIFACT_PATTERNS=(
    "$IMAGE_DIR/$YOCTO_IMAGE_NAME-$PROJECT_BRANCH-$YOCTO_MACHINE.swu"
    "$IMAGE_DIR/$YOCTO_IMAGE_NAME-$PROJECT_BRANCH-$YOCTO_MACHINE.spdx.tar.zst"
    "$IMAGE_DIR/$YOCTO_IMAGE_NAME-$PROJECT_BRANCH-$YOCTO_MACHINE.$YOCTO_IMAGE_EXTENSION"
)
log "Yocto build completed. Verifying expected Yocto artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Consolidating Yocto artifacts into shared staging area..."
collect_artifacts_by_patterns "yocto" "${ARTIFACT_PATTERNS[@]}"

end_timer "Yocto"
