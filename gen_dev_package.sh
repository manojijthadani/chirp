#!/usr/bin/env bash
set -euo pipefail

usage() {
    echo "Usage: $0 <BUILD_DIR> [OUTPUT_ZIP]" 1>&2
    echo "  BUILD_DIR   Absolute or relative path to the build folder containing lib/" 1>&2
    echo "  OUTPUT_ZIP  Optional output zip path (default: <BUILD_DIR>/chirp-dev.zip)" 1>&2
}

if [[ $# -lt 1 ]]; then
    usage
    exit 1
fi

# Resolve absolute paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$1"
if [[ ! "$BUILD_DIR" = /* ]]; then
  BUILD_DIR="$(cd "$BUILD_DIR" 2>/dev/null && pwd)"
fi

if [[ -z "${BUILD_DIR:-}" || ! -d "$BUILD_DIR" ]]; then
    echo "Error: BUILD_DIR does not exist: $1" 1>&2
    exit 1
fi

OUTPUT_ZIP="${2:-"$BUILD_DIR/chirp-dev.zip"}"
if [[ ! "$OUTPUT_ZIP" = /* ]]; then
  OUTPUT_ZIP="$(cd "$(dirname "$OUTPUT_ZIP")" 2>/dev/null || echo "$BUILD_DIR"; pwd)/$(basename "$OUTPUT_ZIP")"
fi

INC_DIR="$SCRIPT_DIR/inc"
DOCS_DIR="$SCRIPT_DIR/docs"
LIB_DIR="$BUILD_DIR/lib"
README_FILE="$SCRIPT_DIR/README.md"

if [[ ! -d "$INC_DIR" ]]; then
    echo "Error: inc/ directory not found at $INC_DIR" 1>&2
    exit 1
fi

if [[ ! -d "$LIB_DIR" ]]; then
    echo "Error: lib/ directory not found in build dir: $LIB_DIR" 1>&2
    exit 1
fi

if ! command -v zip >/dev/null 2>&1; then
    echo "Error: 'zip' utility not found. Please install zip." 1>&2
    exit 1
fi

STAGING_DIR="$(mktemp -d)"
trap 'rm -rf "$STAGING_DIR"' EXIT

mkdir -p "$STAGING_DIR/chirp"

# Copy lib
cp -a "$LIB_DIR" "$STAGING_DIR/chirp/"

# Copy headers
cp -a "$INC_DIR" "$STAGING_DIR/chirp/"

# Copy docs if present
if [[ -d "$DOCS_DIR" ]]; then
    cp -a "$DOCS_DIR" "$STAGING_DIR/chirp/"
fi

# Copy README if present
if [[ -f "$README_FILE" ]]; then
    cp "$README_FILE" "$STAGING_DIR/chirp/"
fi

# Create the zip
(
  cd "$STAGING_DIR" >/dev/null
  # Ensure output directory exists
  mkdir -p "$(dirname "$OUTPUT_ZIP")"
  zip -rq "$OUTPUT_ZIP" chirp
)

echo "Package created: $OUTPUT_ZIP"


