#!/bin/bash

set -euo pipefail

# QMK-Firmware firmware builder
# Builds: crkbd/rev4_1:vial
#
# Native QMK build — no Docker.
# Uses the Arm GNU Toolchain 15.3 explicitly.
# All paths are derived from the script's location.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

QMK_DIR="$REPO_DIR/src/vial-qmk"
FIRMWARE_DIR="$SCRIPT_DIR"

FIRMWARE_NAME="crkbd_rev4_1_vial.uf2"
FIRMWARE_SOURCE="$QMK_DIR/.build/$FIRMWARE_NAME"
ROOT_FIRMWARE="$QMK_DIR/$FIRMWARE_NAME"

BUILD_TARGET="crkbd/rev4_1"
KEYMAP="vial"

ARM_GCC="${ARM_GCC:-}"

if [[ -z "$ARM_GCC" ]]; then
    DEFAULT_ARM_GCC="/Applications/ArmGNUToolchain/15.3.rel2/arm-none-eabi/bin/arm-none-eabi-gcc"

    if [[ -x "$DEFAULT_ARM_GCC" ]]; then
        ARM_GCC="$DEFAULT_ARM_GCC"
    else
        ARM_GCC="$(command -v arm-none-eabi-gcc || true)"
    fi
fi

if [[ -z "$ARM_GCC" || ! -x "$ARM_GCC" ]]; then
    echo "ERROR: arm-none-eabi-gcc was not found."
    echo "Please install the Arm GNU Toolchain 15.3.Rel1 and set ARM_GCC or add it to PATH."
    exit 1
fi

ARM_GCC_VERSION="$("$ARM_GCC" --version | head -n 1)"

if [[ "$ARM_GCC_VERSION" != *"15.3.1"* &&
      "$ARM_GCC_VERSION" != *"16.2.0"* ]]; then
    echo "ERROR: Unsupported Arm GNU Toolchain version; expected GCC 15.3.1 or 16.2.0."
    echo "Detected:"
    echo "  $ARM_GCC_VERSION"
    echo "Compiler:"
    echo "  $ARM_GCC"
    exit 1
fi

ARM_TOOLCHAIN="$(dirname "$ARM_GCC")"
printf '\n=== QMK-FIRMWARE BUILD ===\n\n'
echo "→ Arm GNU Toolchain:"
"$ARM_TOOLCHAIN/arm-none-eabi-gcc" --version | head -n 1

echo "→ QMK directory:"
echo "  $QMK_DIR"

echo "→ Building $BUILD_TARGET:$KEYMAP..."
echo

rm -f "$FIRMWARE_SOURCE"
rm -f "$ROOT_FIRMWARE"
rm -f "$FIRMWARE_DIR/$FIRMWARE_NAME"

cd "$QMK_DIR"

PATH="$ARM_TOOLCHAIN:$PATH" make "$BUILD_TARGET:$KEYMAP"

echo
echo "→ Checking firmware output..."

if [[ ! -f "$FIRMWARE_SOURCE" ]]; then
    echo
    echo "ERROR: Build completed but the expected firmware was not found:"
    echo "  $FIRMWARE_SOURCE"
    echo
    echo "Available UF2 files in .build:"
    find "$QMK_DIR/.build" -maxdepth 1 -type f -name '*.uf2' -print 2>/dev/null || true
    exit 1
fi

cp "$FIRMWARE_SOURCE" "$FIRMWARE_DIR/$FIRMWARE_NAME"

# QMK may copy a firmware file into its own root.
# Keep the repository root clean.
rm -f "$ROOT_FIRMWARE"

echo "→ Firmware copied to:"
echo "  $FIRMWARE_DIR/$FIRMWARE_NAME"

echo
echo "=== BUILD COMPLETE ==="
echo

ls -lh "$FIRMWARE_DIR/$FIRMWARE_NAME"
