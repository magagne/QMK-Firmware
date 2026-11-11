#include QMK_KEYBOARD_H

#include "dynamic_keymap.h"
#include "vial.h"

void td_accent_init(void) {
    vial_tap_dance_entry_t entry = {
        .on_tap = KC_LBRC, /* ^ */
        .on_hold = LSFT(KC_LBRC), /* ¨ */
        .on_double_tap = 0,
        .on_tap_hold = 0,
        .custom_tapping_term = 200,
    };

    dynamic_keymap_set_tap_dance(0, &entry);
}
