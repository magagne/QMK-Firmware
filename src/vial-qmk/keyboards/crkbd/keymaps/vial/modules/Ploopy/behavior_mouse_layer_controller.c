#include "behavior_mouse_layer.h"

#include <stdbool.h>
#include <stdint.h>

#include "host.h"
#include "report.h"

static bool mac_auto_requested;
static bool mac_hold_requested;
static bool mac_latch_requested;
static bool windows_hold_requested;
static bool windows_latch_requested;
static bool windows_caps_known;
static bool windows_caps_on;
static bool windows_caps_suppressed;
static bool mac_layer_owned;
static bool windows_layer_owned;
static uint8_t controller_layer_change_depth;

static uint8_t current_base_layer(void) {
    return get_highest_layer(default_layer_state);
}

static void cancel_auto_timeout(void) {
    behavior_mouse_layer_auto_cancel_timeout();
}

static void reconcile_layer(uint8_t layer, bool requested, bool *owned) {
    bool active = layer_state_is(layer);

    if (requested) {
        if (!active) {
            controller_layer_change_depth++;
            layer_on(layer);
            controller_layer_change_depth--;
            *owned = true;
        }
        return;
    }

    if (*owned && active) {
        controller_layer_change_depth++;
        layer_off(layer);
        controller_layer_change_depth--;
    }
    *owned = false;
}

static void reconcile(void) {
    uint8_t base_layer = current_base_layer();
    bool mac_base = base_layer == PLOOPY_MAC_BASE_LAYER;
    bool windows_base = base_layer == PLOOPY_WINDOWS_BASE_LAYER;
    bool mac_requested = mac_base &&
        (mac_auto_requested || mac_hold_requested || mac_latch_requested);
    bool windows_caps_requested = windows_base && windows_caps_known &&
        windows_caps_on && !windows_caps_suppressed;
    bool windows_requested = windows_base &&
        (windows_caps_requested || windows_hold_requested ||
         windows_latch_requested);

    behavior_manual_temporary_dpi_set(
        false, mac_base && (mac_hold_requested || mac_latch_requested));
    behavior_manual_temporary_dpi_set(
        true, windows_base &&
                  (windows_hold_requested || windows_latch_requested));

    reconcile_layer(PLOOPY_MAC_MOUSE_LAYER, mac_requested, &mac_layer_owned);
    reconcile_layer(
        PLOOPY_WINDOWS_MOUSE_LAYER, windows_requested, &windows_layer_owned);
}

bool behavior_mouse_layer_auto_set(bool active) {
    if (active) {
        uint8_t base_layer = current_base_layer();
        if (base_layer != PLOOPY_MAC_BASE_LAYER) {
            return false;
        }
        mac_auto_requested = true;
    } else {
        mac_auto_requested = false;
    }

    reconcile();
    return true;
}

void behavior_hold_mouse_layer_update(uint8_t layer, bool pressed) {
    if (layer == PLOOPY_MAC_MOUSE_LAYER) {
        if (pressed) {
            mac_latch_requested = false;
            mac_hold_requested = true;
            mac_auto_requested = false;
            cancel_auto_timeout();
        } else {
            mac_hold_requested = false;
        }
    } else if (layer == PLOOPY_WINDOWS_MOUSE_LAYER) {
        if (pressed) {
            windows_caps_suppressed = true;
            windows_latch_requested = false;
            windows_hold_requested = true;
        } else {
            windows_hold_requested = false;
        }
    }
    reconcile();
}

void behavior_latch_mouse_layer_set(uint8_t layer) {
    if (layer == PLOOPY_MAC_MOUSE_LAYER) {
        mac_latch_requested = true;
        mac_auto_requested = false;
        cancel_auto_timeout();
    } else if (layer == PLOOPY_WINDOWS_MOUSE_LAYER) {
        windows_latch_requested = true;
    }

    reconcile();
}

void behavior_unlatch_mouse_layer(uint8_t mouse_layer, uint8_t base_layer) {
    if (mouse_layer == PLOOPY_MAC_MOUSE_LAYER) {
        mac_auto_requested = false;
        mac_hold_requested = false;
        mac_latch_requested = false;
        cancel_auto_timeout();
    } else if (mouse_layer == PLOOPY_WINDOWS_MOUSE_LAYER) {
        windows_hold_requested = false;
        windows_latch_requested = false;
        if (current_base_layer() == PLOOPY_WINDOWS_BASE_LAYER) {
            windows_caps_suppressed = true;
        }
    }

    if (current_base_layer() != base_layer) {
        default_layer_set((layer_state_t)1 << base_layer);
    }
    reconcile();
}

static void clear_external_layer_request(uint8_t layer, bool active) {
    if (layer == PLOOPY_MAC_MOUSE_LAYER) {
        if (active) {
            mac_auto_requested = false;
            mac_hold_requested = true;
            mac_latch_requested = false;
            mac_layer_owned = true;
            cancel_auto_timeout();
        } else {
            mac_auto_requested = false;
            mac_hold_requested = false;
            mac_latch_requested = false;
            cancel_auto_timeout();
        }
    } else if (layer == PLOOPY_WINDOWS_MOUSE_LAYER) {
        if (active) {
            windows_caps_suppressed = true;
            windows_hold_requested = true;
            windows_latch_requested = false;
            windows_layer_owned = true;
        } else {
            if (current_base_layer() == PLOOPY_WINDOWS_BASE_LAYER) {
                windows_caps_suppressed = true;
            }
            windows_hold_requested = false;
            windows_latch_requested = false;
        }
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    if (controller_layer_change_depth == 0) {
        bool mac_before = layer_state_is(PLOOPY_MAC_MOUSE_LAYER);
        bool mac_after = layer_state_cmp(state, PLOOPY_MAC_MOUSE_LAYER);
        bool windows_before = layer_state_is(PLOOPY_WINDOWS_MOUSE_LAYER);
        bool windows_after = layer_state_cmp(state, PLOOPY_WINDOWS_MOUSE_LAYER);

        if (mac_before != mac_after) {
            clear_external_layer_request(PLOOPY_MAC_MOUSE_LAYER, mac_after);
        }
        if (windows_before != windows_after) {
            clear_external_layer_request(
                PLOOPY_WINDOWS_MOUSE_LAYER, windows_after);
        }
    }

    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    uint8_t base_layer = get_highest_layer(state);

    if (base_layer == PLOOPY_WINDOWS_BASE_LAYER) {
        mac_auto_requested = false;
        mac_hold_requested = false;
        mac_latch_requested = false;
        cancel_auto_timeout();
        windows_caps_on = (host_keyboard_leds() & (1 << 1)) != 0;
        windows_caps_known = true;
        windows_caps_suppressed = false;
    } else {
        windows_hold_requested = false;
        windows_latch_requested = false;
        windows_caps_known = false;
        windows_caps_on = false;
        windows_caps_suppressed = false;
    }

    if (base_layer != PLOOPY_MAC_BASE_LAYER) {
        mac_auto_requested = false;
        mac_hold_requested = false;
        mac_latch_requested = false;
        cancel_auto_timeout();
    }

    return state;
}

bool led_update_user(led_t led_state) {
    if (current_base_layer() == PLOOPY_WINDOWS_BASE_LAYER) {
        if (!windows_caps_known || led_state.caps_lock != windows_caps_on) {
            windows_caps_suppressed = false;
        }
        windows_caps_known = true;
        windows_caps_on = led_state.caps_lock;
        reconcile();
    }

    return true;
}

void mouse_layer_controller_task(void) {
    behavior_mouse_layer_kp_or_hold_task();
    behavior_mouse_layer_kp_or_latch_task();
    reconcile();
}

bool mouse_layer_process_record(uint16_t keycode, keyrecord_t *record) {
    if (behavior_mouse_layer_kp_or_hold_process(keycode, record) ||
        behavior_mouse_layer_kp_or_latch_process(keycode, record) ||
        behavior_mouse_layer_hold_process(keycode, record) ||
        behavior_mouse_layer_latch_process(keycode, record) ||
        behavior_mouse_layer_unlatch_process(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case AppleGlobe:
            host_consumer_send(record->event.pressed
                                   ? AC_NEXT_KEYBOARD_LAYOUT_SELECT
                                   : 0);
            return false;
        default:
            return true;
    }
}
