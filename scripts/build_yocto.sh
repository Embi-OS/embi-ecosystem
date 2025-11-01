#!/bin/bash
set -e

# Yocto/manifest defaults (can be overridden in build_yocto.conf)
YOCTO_DIR="$PROJECT_PARENT/_YoctoBuilds"
YOCTO_MANIFEST_URL="git@gitlab.voh.local:projects-ecosysteme/voh-os/voh-manifest.git"
YOCTO_MANIFEST_XML="stable.xml"
YOCTO_MACHINE="apalis-imx8"
YOCTO_TARGET="b2qt-image-swu"
REPO_BIN="$HOME/.local/bin/repo"
BITBAKE_EXTRA_ARGS=""

# Load common config and helpers
. "$(dirname "$0")/common.sh"

# Expand leading ~ if present in paths
YOCTO_DIR="${YOCTO_DIR/#\~/$HOME}"
REPO_BIN="${REPO_BIN/#\~/$HOME}"

start_timer "Yocto"

# Ensure directories
ensure_dir "$YOCTO_DIR"

# Ensure repo tool is available
if [ ! -x "$REPO_BIN" ]; then
    log "Installing repo tool to $REPO_BIN"
    mkdir -p "$(dirname "$REPO_BIN")"
    curl -sSL https://storage.googleapis.com/git-repo-downloads/repo -o "$REPO_BIN"
    chmod a+rx "$REPO_BIN"
fi

# Initialize repo
log "Initializing Yocto repo (manifest: $YOCTO_MANIFEST_URL, file: $YOCTO_MANIFEST_XML)"
run_cmd bash -lc "cd \"$YOCTO_DIR\" && \"$REPO_BIN\" init -u \"$YOCTO_MANIFEST_URL\" -m \"$YOCTO_MANIFEST_XML\""
run_cmd bash -lc "cd \"$YOCTO_DIR\" && \"$REPO_BIN\" sync"

# Construct the yocto build command sequence
YOCTO_ENV_CMD="export MACHINE=\"$YOCTO_MACHINE\"; source ./setup-environment.sh;"
YOCTO_ENV_CMD+=" export BB_ENV_PASSTHROUGH_ADDITIONS=\"\$BB_ENV_PASSTHROUGH_ADDITIONS PRODUCT_VERSION\";"
YOCTO_ENV_CMD+=" export BB_ENV_PASSTHROUGH_ADDITIONS=\"\$BB_ENV_PASSTHROUGH_ADDITIONS PRODUCT_VERSION_NAME\";"
YOCTO_ENV_CMD+=" export BB_ENV_PASSTHROUGH_ADDITIONS=\"\$BB_ENV_PASSTHROUGH_ADDITIONS PRODUCT_IMAGE_BRANCH\";"
YOCTO_ENV_CMD+=" export PRODUCT_VERSION=\"$PROJECT_VERSION\";"
YOCTO_ENV_CMD+=" export PRODUCT_VERSION_NAME=\"$PROJECT_DESCRIPTION\";"
YOCTO_ENV_CMD+=" export PRODUCT_IMAGE_BRANCH=\"$PROJECT_BRANCH\";"

BITBAKE_CMD="bitbake $YOCTO_TARGET ${BITBAKE_EXTRA_ARGS}";

# Run the combined yocto build command in a single subshell to preserve the sourced environment
run_cmd --full bash -lc "cd \"$YOCTO_DIR\"; $YOCTO_ENV_CMD $BITBAKE_CMD"

# Print typical output locations (best effort guess)
YOCTO_BUILD_DIR_GUESS="$YOCTO_DIR/build-$YOCTO_MACHINE"
IMAGE_DIR="$YOCTO_BUILD_DIR_GUESS/tmp/deploy/images/$YOCTO_MACHINE"
QBSP_DIR="$YOCTO_BUILD_DIR_GUESS/tmp/deploy/qbsp"

# Verify that expected artifacts were produced (patterns are globs)
ARTIFACT_PATTERNS=(
    "$IMAGE_DIR/$YOCTO_IMAGE_NAME-$PROJECT_BRANCH-$YOCTO_MACHINE.swu"
    "$IMAGE_DIR/$YOCTO_IMAGE_NAME-$PROJECT_BRANCH-$YOCTO_MACHINE.$YOCTO_IMAGE_EXTENSION"
)
log "Yocto build completed. Verifying expected Yocto artifacts..."
verify_artifacts "${ARTIFACT_PATTERNS[@]}"
log "Consolidating Yocto artifacts into shared staging area..."
collect_artifacts_by_patterns "yocto" "${ARTIFACT_PATTERNS[@]}"

end_timer "Yocto"
