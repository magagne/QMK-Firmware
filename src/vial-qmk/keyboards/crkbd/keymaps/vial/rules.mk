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

SRC += modules/ploopy/drag_scroll/drag_scroll.c
SRC += modules/ploopy/auto_mouse_layer/auto_mouse_layer.c
SRC += modules/ploopy/mouse_layer/mouse_layer_controller.c
SRC += modules/ploopy/mouse_layer/temporary_dpi.c

SRC += modules/tap_dance/tap_dance.c
