#include "behavior_mouse_layer.h"

#include <stdbool.h>
#include <stdint.h>

#include "raw_hid.h"

#define PLOOPY_HID_AUTO_MOUSE_LAYER 0x41
#define PLOOPY_HID_AUTO_MOUSE_LAYER_VERSION 0x01

static uint16_t auto_mouse_layer_last_activity;
static bool auto_mouse_layer_timeout_active;

void behavior_mouse_layer_auto_cancel_timeout(void) {
    auto_mouse_layer_timeout_active = false;
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (length < 2 ||
        data[0] != PLOOPY_HID_AUTO_MOUSE_LAYER ||
        data[1] != PLOOPY_HID_AUTO_MOUSE_LAYER_VERSION) {
        return;
    }

    if (behavior_mouse_layer_auto_set(true)) {
        auto_mouse_layer_last_activity = timer_read();
        auto_mouse_layer_timeout_active = true;
    }
}

void auto_mouse_layer_task(void) {
    if (auto_mouse_layer_timeout_active &&
        timer_elapsed(auto_mouse_layer_last_activity) >=
            PLOOPY_AUTO_MOUSE_LAYER_TIMEOUT) {
        auto_mouse_layer_timeout_active = false;
        (void)behavior_mouse_layer_auto_set(false);
    }

    mouse_layer_controller_task();
}
