#include "behavior_mouse_layer.h"

#include <stdint.h>

#include "raw_hid.h"

static bool drag_scroll_enabled = false;

bool behavior_raw_drag_scroll_process_record(uint16_t keycode,
                                             keyrecord_t *record) {
    switch (keycode) {
        case HID_DragScroll:
            drag_scroll_set(record->event.pressed);
            return true;
        case LED_DragScroll:
            tap_code(KC_SCRL);
            return true;
        default:
            return false;
    }
}

static void drag_scroll_send(uint8_t command) {
    uint8_t data[32] = {0};
    data[0] = command;
    raw_hid_send(data, sizeof(data));
}

void drag_scroll_init(void) {
    drag_scroll_enabled = false;
}

void drag_scroll_set(bool enabled) {
    if (drag_scroll_enabled == enabled) {
        return;
    }

    drag_scroll_enabled = enabled;

    if (enabled) {
        drag_scroll_send(0x53);
    } else {
        drag_scroll_send(0x73);
    }
}

void drag_scroll_task(void) {
}