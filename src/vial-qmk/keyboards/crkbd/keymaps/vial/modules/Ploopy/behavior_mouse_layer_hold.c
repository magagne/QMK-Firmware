#include "behavior_mouse_layer.h"

bool behavior_mouse_layer_hold_process(uint16_t keycode, keyrecord_t *record) {
    uint8_t layer;
    switch (keycode) {
        case HoldMouseLayer_mac:
            layer = PLOOPY_MAC_MOUSE_LAYER;
            break;
        case HoldMouseLayer_win:
            layer = PLOOPY_WINDOWS_MOUSE_LAYER;
            break;
        default:
            return false;
    }

    behavior_hold_mouse_layer_update(layer, record->event.pressed);
    return true;
}
