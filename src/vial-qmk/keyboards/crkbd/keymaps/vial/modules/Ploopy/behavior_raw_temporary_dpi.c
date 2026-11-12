#include "behavior_mouse_layer.h"

#include <stdint.h>

#include "raw_hid.h"
#include "quantum.h"

static bool mac_manual_requested;
static bool mac_standalone_requested;
static bool mac_output_active;
static bool windows_manual_requested;
static bool windows_standalone_requested;
static bool windows_output_active;

bool behavior_raw_temporary_dpi_process_record(uint16_t keycode,
                                               keyrecord_t *record) {
    switch (keycode) {
        case HID_TempDPI:
            behavior_temporary_dpi_set(false, record->event.pressed);
            return true;
        case LED_TempDPI:
            behavior_temporary_dpi_set(true, record->event.pressed);
            return true;
        default:
            return false;
    }
}

static void send_mac_state(bool active) {
    uint8_t data[32] = {0};
    data[0] = active ? 0x54 : 0x74;
    raw_hid_send(data, sizeof(data));
}

static void update_state(bool windows_host) {
    if (windows_host) {
        bool desired = windows_manual_requested || windows_standalone_requested;
        if (desired != windows_output_active) {
            tap_code(KC_NUM);
            windows_output_active = desired;
        }
        return;
    }

    bool desired = mac_manual_requested || mac_standalone_requested;
    if (desired != mac_output_active) {
        send_mac_state(desired);
        mac_output_active = desired;
    }
}

void behavior_manual_temporary_dpi_set(bool windows_host, bool active) {
    if (windows_host) {
        windows_manual_requested = active;
    } else {
        mac_manual_requested = active;
    }
    update_state(windows_host);
}

void behavior_temporary_dpi_set(bool windows_host, bool active) {
    if (windows_host) {
        windows_standalone_requested = active;
    } else {
        mac_standalone_requested = active;
    }
    update_state(windows_host);
}
