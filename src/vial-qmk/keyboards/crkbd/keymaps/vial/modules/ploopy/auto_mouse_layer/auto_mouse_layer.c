#include QMK_KEYBOARD_H

#include "auto_mouse_layer.h"
#include "raw_hid.h"


/* ---------------------------------------------------------------------------
 * Custom keycodes
 * ------------------------------------------------------------------------- */

#define PLOOPY_HID_AUTO_MOUSE_LAYER         0x41
#define PLOOPY_HID_AUTO_MOUSE_LAYER_VERSION 0x01
#define PLOOPY_AUTO_MOUSE_LAYER     3
#define PLOOPY_AUTO_MOUSE_WINDOWS_LAYER 7

static bool auto_mouse_layer_active = false;
static bool auto_mouse_windows_active = false;


static void auto_mouse_layer_activate(void) {
    if (!auto_mouse_layer_active) {
        layer_on(PLOOPY_AUTO_MOUSE_LAYER);
        auto_mouse_layer_active = true;
    }

}

static void auto_mouse_windows_set_state(bool active) {
    if (active == auto_mouse_windows_active) {
        return;
    }

    if (active) {
        layer_on(PLOOPY_AUTO_MOUSE_WINDOWS_LAYER);
    } else {
        layer_off(PLOOPY_AUTO_MOUSE_WINDOWS_LAYER);
    }

    auto_mouse_windows_active = active;
}

/*
 * macOS: release the automatically activated Mouse layer.
 */
void auto_mouse_layer_exit_mac(void) {
    layer_off(PLOOPY_AUTO_MOUSE_LAYER);
    auto_mouse_layer_active = false;
}

/*
 * Windows: turn Caps Lock off only if it is currently on,
 * then release the Windows Mouse layer.
 */
void auto_mouse_layer_exit_win(void) {
    if ((host_keyboard_leds() & (1 << 1)) != 0) {
        tap_code(KC_CAPS);
    }

    auto_mouse_windows_set_state(false);
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (length < 2) {
        return;
    }

    if (data[0] != PLOOPY_HID_AUTO_MOUSE_LAYER ||
        data[1] != PLOOPY_HID_AUTO_MOUSE_LAYER_VERSION) {
        return;
    }

    auto_mouse_layer_activate();
}

bool led_update_user(led_t led_state) {
    if (get_highest_layer(default_layer_state) == 4) {
        auto_mouse_windows_set_state(led_state.caps_lock);
    }

    return true;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    uint8_t base_layer = get_highest_layer(state);

    if (base_layer == 4) {
        if (auto_mouse_layer_active) {
            layer_off(PLOOPY_AUTO_MOUSE_LAYER);
            auto_mouse_layer_active = false;
        }

        auto_mouse_windows_set_state(
            (host_keyboard_leds() & (1 << 1)) != 0
        );
    } else if (base_layer == 0) {
        if (auto_mouse_windows_active) {
            auto_mouse_windows_set_state(false);
        }
    } else {
        if (auto_mouse_windows_active) {
            auto_mouse_windows_set_state(false);
        }
        if (auto_mouse_layer_active) {
            layer_off(PLOOPY_AUTO_MOUSE_LAYER);
            auto_mouse_layer_active = false;
        }
    }

    return state;
}

void auto_mouse_layer_task(void) {
    if (get_highest_layer(default_layer_state) != 0) {
        if (auto_mouse_layer_active) {
            layer_off(PLOOPY_AUTO_MOUSE_LAYER);
            auto_mouse_layer_active = false;
        }
        return;
    }

    /*
     * Reconcile ownership after a manual Mouse-layer exit.
     * This allows the next Raw HID movement to activate it again.
     */
    if (auto_mouse_layer_active &&
        !layer_state_is(PLOOPY_AUTO_MOUSE_LAYER)) {
        auto_mouse_layer_active = false;
    }
}

