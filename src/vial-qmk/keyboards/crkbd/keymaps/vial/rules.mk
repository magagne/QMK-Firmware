VIA_ENABLE          = yes
VIAL_ENABLE         = yes
RAW_ENABLE          = yes

LTO_ENABLE          = yes
RGBLIGHT_ENABLE     = yes
RGB_MATRIX_ENABLE   = no
MOUSEKEY_ENABLE     = yes
OLED_ENABLE         = no
OLED_DRIVER         = SSD1306
EXTRAKEY_ENABLE     = yes
COMBO_ENABLE        = yes
TAP_DANCE_ENABLE    = yes
QMK_SETTINGS        = no
ENCODER_MAP_ENABLE = yes

SRC += modules/Ploopy/behavior_raw_drag_scroll.c
SRC += modules/Ploopy/behavior_mouse_layer_auto.c
SRC += modules/Ploopy/behavior_mouse_layer_controller.c
SRC += modules/Ploopy/behavior_mouse_layer_hold.c
SRC += modules/Ploopy/behavior_mouse_layer_latch.c
SRC += modules/Ploopy/behavior_mouse_layer_kp_or_hold.c
SRC += modules/Ploopy/behavior_mouse_layer_kp_or_latch.c
SRC += modules/Ploopy/behavior_mouse_layer_unlatch.c
SRC += modules/Ploopy/behavior_raw_temporary_dpi.c

SRC += modules/tap_dance/tap_dance.c
