#pragma once

#include QMK_KEYBOARD_H

#define PLOOPY_MAC_BASE_LAYER 0
#define PLOOPY_MAC_MOUSE_LAYER 3
#define PLOOPY_WINDOWS_BASE_LAYER 4
#define PLOOPY_WINDOWS_MOUSE_LAYER 7
#define PLOOPY_MOUSE_LAYER_TAPPING_TERM 300

#ifndef PLOOPY_AUTO_MOUSE_LAYER_TIMEOUT
#    define PLOOPY_AUTO_MOUSE_LAYER_TIMEOUT 400
#endif

#define PLOOPY_KP_OR_TAP_KEY_MAX 0x5F
#define PLOOPY_KP_OR_HOLD_MAC(kc) (QK_USER + (kc))
#define PLOOPY_KP_OR_HOLD_WIN(kc) (QK_USER + 0x60 + (kc))
#define PLOOPY_KP_OR_LATCH_MAC(kc) (QK_USER + 0xC0 + (kc))
#define PLOOPY_KP_OR_LATCH_WIN(kc) (QK_USER + 0x120 + (kc))

enum custom_keycodes {
    HID_DragScroll = QK_KB_0,
    LED_DragScroll,
    LCLKx2,
    ExitMouseLayer_mac,
    ExitMouseLayer_win,
    HoldMouseLayer_mac,
    HoldMouseLayer_win,
    LatchMouseLayer_mac,
    LatchMouseLayer_win,
    UnlatchMouseLayer_mac,
    UnlatchMouseLayer_win,
    KpOrHoldMouseLayer_mac_N,
    KpOrHoldMouseLayer_win_N,
    KpOrLatchMouseLayer_mac_N,
    KpOrLatchMouseLayer_win_N,
    HID_TempDPI,
    LED_TempDPI,
    AppleGlobe,
};

bool mouse_layer_process_record(uint16_t keycode, keyrecord_t *record);
void mouse_layer_controller_task(void);
void mouse_layer_kp_or_task(void);
bool behavior_mouse_layer_auto_set(bool active);
void behavior_mouse_layer_auto_cancel_timeout(void);
void behavior_hold_mouse_layer_update(uint8_t layer, bool pressed);
void behavior_latch_mouse_layer_set(uint8_t layer);
void behavior_unlatch_mouse_layer(uint8_t mouse_layer, uint8_t base_layer);
void behavior_manual_temporary_dpi_set(bool windows_host, bool active);
void behavior_temporary_dpi_set(bool windows_host, bool active);
