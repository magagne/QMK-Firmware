# QMK-Firmware

Custom VIAL firmware for the Corne Rev4.1.

## Overview

This repository contains a VIAL-enabled QMK firmware build for the Corne Rev4.1.

The project separates firmware functionality from the compiled default layout:

- QMK provides the firmware and custom functionality.
- `keymap.c` provides the compiled default keyboard layout.
- VIAL can edit and persist the active keymap in EEPROM.
- A `.vil` export can be used to save or transfer a VIAL layout.

The compiled default translates the ZMK layer key assignments for layers 0-9,
including its combos and macros. QMK-specific layers 10 and 11 are preserved.

## Firmware target

    crkbd/rev4_1:vial

## Repository structure

    Firmware/Crkbd/
    ├── BuildFirmware.sh
    └── crkbd_rev4_1_vial.uf2

    src/vial-qmk/keyboards/crkbd/keymaps/vial/
    ├── keymap.c
    ├── rules.mk
    └── modules/
        ├── Ploopy/
        │   ├── behavior_mouse_layer.h
        │   ├── behavior_mouse_layer_auto.c
        │   ├── behavior_mouse_layer_controller.c
        │   ├── behavior_mouse_layer_hold.c
        │   ├── behavior_mouse_layer_kp_or_hold.c
        │   ├── behavior_mouse_layer_kp_or_latch.c
        │   ├── behavior_mouse_layer_latch.c
        │   ├── behavior_mouse_layer_unlatch.c
        │   ├── behavior_raw_drag_scroll.c
        │   └── behavior_raw_temporary_dpi.c
        └── tap_dance/
            ├── tap_dance.c
            └── tap_dance.h

The custom firmware modules are kept with the VIAL keymap so their implementation and integration points remain together.

## Building

The recommended build method is the project build script.

From the repository root:

    ./Firmware/Crkbd/BuildFirmware.sh

The script builds:

    crkbd/rev4_1:vial

The generated firmware is:

    Firmware/Crkbd/crkbd_rev4_1_vial.uf2

## Manual build

The firmware can also be built directly from the bundled QMK source.

From the repository root:

    cd src/vial-qmk
    make crkbd/rev4_1:vial

The ARM GNU toolchain must be available in the system PATH.

No developer-specific filesystem path is required.

## Flashing

After building, flash:

    Firmware/Crkbd/crkbd_rev4_1_vial.uf2

to the Corne Rev4.1 using the normal RP2040 UF2 bootloader procedure.

After flashing, open VIAL. To use a saved layout, load its `.vil` export; to
use the compiled defaults, reset the dynamic keymap. Existing VIAL EEPROM
layouts remain active across firmware flashes.

## VIAL layout

The compiled default layout in `keymap.c` translates the ZMK layer key
assignments for layers 0-9. ZMK combos and macros are not ported. QMK layers
10 and 11 remain custom. The QMK `ex2` layout has four more physical positions
than the ZMK layout; those additional positions are left unassigned.

VIAL can override and persist the compiled keymap in EEPROM. Reflashing the
firmware does not replace an already-saved Vial keymap; load a `.vil` export
or reset the dynamic keymap to use the compiled defaults.

## Custom firmware functionality

The firmware provides:

- HID Auto Mouse Layer
- Drag scrolling
- Scroll Lock HID control for DragScroll
- Modified mouse clicks
- QMK mouse keys
- RGB controls
- Encoder support
- Custom firmware keycodes
- Apple Globe consumer key
- ZMK-derived combos and editor macros

The custom firmware modules are located at:

    src/vial-qmk/keyboards/crkbd/keymaps/vial/modules/

## Combos and macros

The eight ZMK combos are installed into Vial's eight combo slots on the first
boot after this firmware update. They remain editable in Vial afterward.
Layer-sensitive combos only trigger on the corresponding Mac or Windows
layers.

The IDE and bookmark macros from ZMK are available as Vial custom keycodes.
The M10-M13 combo triggers execute Vial's editable dynamic macro slots 10-13.
On first initialization with an empty macro buffer, slots 10-13 are seeded with
`Macro_10` through `Macro_13`. Replace those placeholders in Vial's macro
editor. Existing non-empty macro buffers are preserved. Macro contents are
stored unencrypted in keyboard EEPROM, so avoid storing credentials or other
sensitive data. The ZMK Bluetooth-selection macros map to QMK default-layer
changes; this wired QMK target cannot select Bluetooth profiles.

## HID Auto Mouse Layer

HID Auto Mouse Layer automatically enables the mouse layer when the Ploopy Nano-2 reports trackball activity.

The feature has two host paths.

### macOS

On macOS, the Ploopy Nano-2 sends an Auto Mouse Layer activity packet through Raw HID.

The packet uses:

    Byte 0: 0x41
    Byte 1: 0x01

The QMK firmware receives this packet through `raw_hid_receive_kb()` and activates the macOS mouse layer.

For the current layout:

    Base layer 0
    Mouse layer 3
    Timeout: 400 ms
    Practical tuning range: approximately 300–700 ms

The QMK Auto Mouse Layer timeout is **400 ms**, matching the ZMK configuration. It can be overridden with `PLOOPY_AUTO_MOUSE_LAYER_TIMEOUT`; a practical tuning range is approximately **300–700 ms**.

### Windows

On Windows, Auto Mouse Layer uses the Caps Lock LED state as the state signal.

The Ploopy Nano-2 sends a Caps Lock press/release through the host:

    Trackball movement starts  → Caps Lock ON
    Trackball remains moving   → no repeated Caps Lock traffic

Windows reports the resulting Caps Lock LED state back to the keyboard.

The QMK firmware then maps that state to the Windows mouse layer:

    Caps Lock ON  → Windows mouse layer ON
    Caps Lock OFF → Windows mouse layer OFF

For the current layout:

    Base layer 4
    Mouse layer 7

The Windows path is state-based and does not use the QMK Auto Mouse timeout.

### Layer separation

The two Auto Mouse paths intentionally use different signaling mechanisms:

    macOS   → Raw HID activity → layer 3 → 400 ms timeout
    Windows → Caps Lock state  → layer 7 → LED state controls layer

The Windows path is only active while the Windows base layer (layer 4) is selected.

Changing away from the relevant base layer disables the corresponding Auto Mouse state.

## DragScroll

DragScroll is implemented as a separate module:

    src/vial-qmk/keyboards/crkbd/keymaps/vial/modules/Ploopy/

The module provides:

    drag_scroll_init()
    drag_scroll_set()
    drag_scroll_task()

The custom keycodes are:

    HID_DragScroll
    LED_DragScroll

`HID_DragScroll` controls the DragScroll HID transport.

`LED_DragScroll` sends a Scroll Lock toggle to the host. Scroll Lock is reserved for DragScroll state and is not used by HID Auto Mouse Layer.

The DragScroll implementation is independent of the Auto Mouse Layer implementation.

## Tap Dance

Tap Dance is implemented as a separate module:

    src/vial-qmk/keyboards/crkbd/keymaps/vial/modules/tap_dance/

The module provides:

    td_accent_init()

The current implementation configures Vial Tap Dance entry 0.

### Accent Tap Dance

Tap Dance entry 0 provides two behaviors:

    Tap   → [
    Hold  → {

The hold action is implemented as:

    LSFT(KC_LBRC)

Double-tap and tap-hold actions are not configured.

The entry uses a 200 ms tapping term, matching the ZMK hold-tap behavior.

The Tap Dance entry is initialized during keyboard startup by:

    td_accent_init()

The physical key assignment uses:

    TD(0)

The assignment is present on the symbol layers where the Accent Tap Dance is required.

The Tap Dance implementation is independent of DragScroll and HID Auto Mouse Layer.

## Custom keycodes

The firmware currently defines:

    HID_DragScroll
    LED_DragScroll
    HOLD_MOUSE_LAYER_MAC / HOLD_MOUSE_LAYER_WIN
    LATCH_MOUSE_LAYER_MAC / LATCH_MOUSE_LAYER_WIN
    UNLATCH_MOUSE_LAYER_MAC / UNLATCH_MOUSE_LAYER_WIN
    KP_OR_HOLD_MOUSE_LAYER_MAC_N / KP_OR_HOLD_MOUSE_LAYER_WIN_N
    KP_OR_LATCH_MOUSE_LAYER_MAC_N / KP_OR_LATCH_MOUSE_LAYER_WIN_N
    HID_TEMPORARY_DPI / LED_TEMPORARY_DPI
    APPLE_GLOBE

These keycodes implement firmware functionality.

The compiled default assigns custom keycodes in `keymap.c`. VIAL can change
those assignments and persist them in EEPROM.

The mouse-layer controller arbitrates automatic and manual activation for
layers 3 (Mac) and 7 (Windows). Manual hold/latch requests signal temporary
DPI only for the selected host; automatic activation does not. The `KP_OR_*_N`
keys tap `N` when released before the 300 ms tapping term, or hold/latch the
host's Mouse layer when held through the term. The standalone temporary-DPI
keys use Raw HID on Mac and a Num Lock pulse on Windows.

The `PLOOPY_KP_OR_HOLD_MAC(kc)`, `PLOOPY_KP_OR_HOLD_WIN(kc)`,
`PLOOPY_KP_OR_LATCH_MAC(kc)`, and `PLOOPY_KP_OR_LATCH_WIN(kc)` macros provide
the same tap-hold behaviors with a caller-selected QMK tap keycode up to
`PLOOPY_KP_OR_TAP_KEY_MAX`. VIAL exposes `N` variants directly.

## Tap Dance

The compiled default assigns `TD(0)` on symbol layers 2 and 6. VIAL can
override those assignments in its EEPROM keymap. Reset the dynamic keymap to
restore the compiled defaults, or load a saved `.vil` export.

## Source of truth

The compiled default keymap and firmware functionality live in:

    src/vial-qmk/keyboards/crkbd/keymaps/vial/

Vial changes are stored in EEPROM on the keyboard. A `.vil` export can be
stored separately to back up or transfer those changes.

## Development workflow

For a firmware change:

    ./Firmware/Crkbd/BuildFirmware.sh

Then:

1. Flash the generated `.uf2`.
2. Open VIAL.
3. If restoring a saved VIAL layout, load its `.vil` export; otherwise reset
   the dynamic keymap to use the compiled defaults.
4. Verify the layout.
5. Test the firmware functionality.

When changing a custom firmware module, keep its header and source together under:

    src/vial-qmk/keyboards/crkbd/keymaps/vial/modules/

## Hardware

Target hardware:

    Corne Rev4.1

QMK/VIAL target:

    crkbd/rev4_1:vial

## Build script

The project build entry point is:

    Firmware/Crkbd/BuildFirmware.sh

The script uses repository-relative paths and does not depend on a developer-specific filesystem location.
