#include "behavior_mouse_layer.h"

#include <stdbool.h>
#include <stdint.h>

struct kp_or_hold_state {
    bool active;
    bool triggered;
    uint8_t layer;
    uint8_t tap_keycode;
    uint16_t started_at;
};

static struct kp_or_hold_state kp_or_hold_states[MATRIX_ROWS][MATRIX_COLS];

static bool kp_or_hold_keycode(uint16_t keycode, uint8_t *layer,
                               uint8_t *tap_keycode) {
    switch (keycode) {
        case KpOrHoldMouseLayer_mac_N:
            *layer = PLOOPY_MAC_MOUSE_LAYER;
            *tap_keycode = KC_N;
            return true;
        case KpOrHoldMouseLayer_win_N:
            *layer = PLOOPY_WINDOWS_MOUSE_LAYER;
            *tap_keycode = KC_N;
            return true;
    }

    if (keycode < QK_USER || keycode >= QK_USER + 0xC0) {
        return false;
    }

    uint16_t offset = keycode - QK_USER;
    uint8_t host = offset / 0x60;
    if (host > 1) {
        return false;
    }

    *layer = host == 0 ? PLOOPY_MAC_MOUSE_LAYER : PLOOPY_WINDOWS_MOUSE_LAYER;
    *tap_keycode = offset % 0x60;
    return true;
}

bool behavior_mouse_layer_kp_or_hold_process(uint16_t keycode,
                                             keyrecord_t *record) {
    uint8_t layer;
    uint8_t tap_keycode;
    if (!kp_or_hold_keycode(keycode, &layer, &tap_keycode)) {
        return false;
    }

    uint8_t row = record->event.key.row;
    uint8_t col = record->event.key.col;
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        return true;
    }

    struct kp_or_hold_state *state = &kp_or_hold_states[row][col];
    if (record->event.pressed) {
        state->active = true;
        state->triggered = false;
        state->layer = layer;
        state->tap_keycode = tap_keycode;
        state->started_at = timer_read();
    } else if (state->active) {
        if (!state->triggered &&
            timer_elapsed(state->started_at) >=
                PLOOPY_MOUSE_LAYER_TAPPING_TERM) {
            state->triggered = true;
            behavior_hold_mouse_layer_update(state->layer, true);
        }

        if (state->triggered) {
            behavior_hold_mouse_layer_update(state->layer, false);
        } else {
            tap_code(state->tap_keycode);
        }
        state->active = false;
    }

    return true;
}

void behavior_mouse_layer_kp_or_hold_task(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            struct kp_or_hold_state *state = &kp_or_hold_states[row][col];
            if (!state->active || state->triggered ||
                timer_elapsed(state->started_at) <
                    PLOOPY_MOUSE_LAYER_TAPPING_TERM) {
                continue;
            }

            state->triggered = true;
            behavior_hold_mouse_layer_update(state->layer, true);
        }
    }
}
