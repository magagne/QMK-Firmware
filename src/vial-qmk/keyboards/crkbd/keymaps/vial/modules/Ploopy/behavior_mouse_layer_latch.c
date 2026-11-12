#include "behavior_mouse_layer.h"

bool behavior_mouse_layer_latch_process(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return keycode == LatchMouseLayer_mac ||
               keycode == LatchMouseLayer_win;
    }

    switch (keycode) {
        case LatchMouseLayer_mac:
            behavior_latch_mouse_layer_set(PLOOPY_MAC_MOUSE_LAYER);
            return true;
        case LatchMouseLayer_win:
            behavior_latch_mouse_layer_set(PLOOPY_WINDOWS_MOUSE_LAYER);
            return true;
        default:
            return false;
    }
}
