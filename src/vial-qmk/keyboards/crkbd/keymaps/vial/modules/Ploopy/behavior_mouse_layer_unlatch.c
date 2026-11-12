#include "behavior_mouse_layer.h"

bool behavior_mouse_layer_unlatch_process(uint16_t keycode,
                                          keyrecord_t *record) {
    uint8_t mouse_layer;
    uint8_t base_layer;
    switch (keycode) {
        case UnlatchMouseLayer_mac:
        case ExitMouseLayer_mac:
            mouse_layer = PLOOPY_MAC_MOUSE_LAYER;
            base_layer = PLOOPY_MAC_BASE_LAYER;
            break;
        case UnlatchMouseLayer_win:
        case ExitMouseLayer_win:
            mouse_layer = PLOOPY_WINDOWS_MOUSE_LAYER;
            base_layer = PLOOPY_WINDOWS_BASE_LAYER;
            break;
        default:
            return false;
    }

    if (record->event.pressed) {
        behavior_unlatch_mouse_layer(mouse_layer, base_layer);
    }
    return true;
}
