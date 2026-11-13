#include QMK_KEYBOARD_H

#include "dynamic_keymap.h"
#include "eeconfig.h"
#include "modules/Ploopy/behavior_mouse_layer.h"
#include "modules/tap_dance/tap_dance.h"
#include "print.h"
#include "process_combo.h"
#include "vial.h"

#define ZMK_COMBO_DEFAULTS_MAGIC 0x5A4D4B43UL
#define VIAL_COMBO_AND_MACRO_DEFAULTS_MAGIC 0x5A4D4B44UL
#define MAC_SCREENSHOT_KEY LGUI(LSFT(KC_4))
#define WINDOWS_SCREENSHOT_KEY KC_PRINT_SCREEN

enum combo_internal_keycodes {
    HOST_LINE_END_KEY = QK_USER + 0x180,
    HOST_SCREENSHOT_KEY,
    COMBO_BRIGHTNESS_UP,
    COMBO_BRIGHTNESS_DOWN,
};

static const vial_combo_entry_t zmk_combo_defaults[VIAL_COMBO_ENTRIES] = {
    {.input = {LSFT(KC_1), RALT(KC_LBRC)}, .output = Macro_M10},
    {.input = {RALT(KC_8), LSFT(KC_2)}, .output = Macro_M11},
    {.input = {KC_INSERT, HOST_LINE_END_KEY}, .output = Macro_M12},
    /* This shared key represents Cmd+Shift+4 on Mac and Print Screen on Windows. */
    {.input = {HOST_SCREENSHOT_KEY, KC_CAPS_LOCK}, .output = Macro_M13},
    {.input = {KC_J, KC_M}, .output = COMBO_BRIGHTNESS_UP},
    {.input = {KC_K, KC_M}, .output = COMBO_BRIGHTNESS_DOWN},
    {.input = {KC_B, KC_G}, .output = KC_AUDIO_VOL_UP},
    {.input = {KC_V, KC_G}, .output = KC_AUDIO_VOL_DOWN},
};

extern combo_t key_combos[VIAL_COMBO_ENTRIES];
extern uint16_t key_combos_keys[VIAL_COMBO_ENTRIES][5];

static uint8_t current_keymap_layer(void) {
    return get_highest_layer(layer_state | default_layer_state);
}

static bool vial_macro_buffer_is_empty(void) {
    uint16_t buffer_size = dynamic_keymap_macro_get_buffer_size();
    uint8_t buffer[16];

    for (uint16_t offset = 0; offset < buffer_size;) {
        uint16_t chunk_size = buffer_size - offset;
        if (chunk_size > sizeof(buffer)) {
            chunk_size = sizeof(buffer);
        }

        dynamic_keymap_macro_get_buffer(offset, chunk_size, buffer);
        for (uint16_t i = 0; i < chunk_size; i++) {
            if (buffer[i] != 0) {
                return false;
            }
        }

        offset += chunk_size;
    }

    return true;
}

static bool vial_combo_entries_are_empty(void) {
    for (uint8_t combo_index = 0; combo_index < VIAL_COMBO_ENTRIES;
         combo_index++) {
        vial_combo_entry_t entry;
        if (dynamic_keymap_get_combo(combo_index, &entry) != 0) {
            uprintf("Failed to read Vial combo %u\n", combo_index);
            return false;
        }

        if (entry.output != KC_NO) {
            return false;
        }
        for (uint8_t input_index = 0; input_index < 4; input_index++) {
            if (entry.input[input_index] != KC_NO) {
                return false;
            }
        }
    }

    return true;
}

static bool install_vial_macro_defaults(void) {
    static uint8_t default_macros[] =
        "\0\0\0\0\0\0\0\0\0\0Macro_10\0Macro_11\0Macro_12\0Macro_13";
    uint16_t default_size = sizeof(default_macros);

    if (dynamic_keymap_macro_get_count() < 14) {
        uprintf("Vial macro defaults require at least 14 macro slots\n");
        return false;
    }
    if (dynamic_keymap_macro_get_buffer_size() < default_size) {
        uprintf("Insufficient EEPROM space for Vial macro defaults\n");
        return false;
    }
    if (!vial_macro_buffer_is_empty()) {
        return true;
    }

    dynamic_keymap_macro_set_buffer(0, default_size, default_macros);
    return true;
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo,
                          uint16_t keycode, keyrecord_t *record) {
    uint8_t layer = current_keymap_layer();

    (void)combo;
    (void)keycode;
    (void)record;

    switch (combo_index) {
        case 0:
        case 1:
            return layer == 2 || layer == 6;
        case 2:
            return layer == 1 || layer == 5;
        case 3:
            return layer == 1 || layer == 5;
        case 4:
        case 5:
        case 6:
        case 7:
            return layer == 0 || layer == 4;
        default:
            return true;
    }
}

static void install_zmk_combo_defaults(void) {
    uint32_t user_magic = eeconfig_read_user();
    bool known_magic = user_magic == ZMK_COMBO_DEFAULTS_MAGIC ||
                       user_magic == VIAL_COMBO_AND_MACRO_DEFAULTS_MAGIC;

    if (!known_magic || vial_combo_entries_are_empty()) {
        for (uint8_t combo_index = 0; combo_index < VIAL_COMBO_ENTRIES;
             combo_index++) {
            if (dynamic_keymap_set_combo(combo_index,
                                         &zmk_combo_defaults[combo_index]) != 0) {
                uprintf("Failed to initialize ZMK combo %u\n", combo_index);
                return;
            }

            for (uint8_t input_index = 0; input_index < 4; input_index++) {
                key_combos_keys[combo_index][input_index] =
                    zmk_combo_defaults[combo_index].input[input_index];
            }
            key_combos_keys[combo_index][4] = COMBO_END;
            key_combos[combo_index].keys = key_combos_keys[combo_index];
            key_combos[combo_index].keycode =
                zmk_combo_defaults[combo_index].output;
        }
    }

    if (!install_vial_macro_defaults()) {
        return;
    }

    eeconfig_update_user(VIAL_COMBO_AND_MACRO_DEFAULTS_MAGIC);
}

static void run_defined_macro(uint16_t keycode) {
    switch (keycode) {
        case Macro_M00_Outline_CollapseAll:
            tap_code16(LCTL(KC_M));
            tap_code16(LCTL(KC_A));
            break;
        case Macro_M01_Outline_ToggleCurrent:
            tap_code16(LCTL(KC_M));
            tap_code16(LCTL(KC_M));
            break;
        case Macro_M02_Outline_ExpandAll:
            tap_code16(LCTL(KC_M));
            tap_code16(LCTL(KC_L));
            break;
        case Macro_M03_Bookmark_Previous:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_P));
            break;
        case Macro_M04_Bookmark_Toggle:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_K));
            break;
        case Macro_M05_Bookmark_ClearAll:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_L));
            break;
        case Macro_M06_Bookmark_Next:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_N));
            break;
        case Macro_M07_Comment:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_C));
            break;
        case Macro_M08_UnComment:
            tap_code16(LCTL(KC_K));
            tap_code16(LCTL(KC_U));
            break;
        case Macro_M10:
            dynamic_keymap_macro_send(10);
            break;
        case Macro_M11:
            dynamic_keymap_macro_send(11);
            break;
        case Macro_M12:
            dynamic_keymap_macro_send(12);
            break;
        case Macro_M13:
            dynamic_keymap_macro_send(13);
            break;
    }
}

static void run_mac_brightness_hotkey(uint8_t function_key) {
    register_mods(MOD_LCTL | MOD_LSFT | MOD_LALT | MOD_LGUI);
    tap_code(function_key);
    unregister_mods(MOD_LCTL | MOD_LSFT | MOD_LALT | MOD_LGUI);
}

static void run_brightness_up_combo(void) {
    if (get_highest_layer(default_layer_state) == PLOOPY_MAC_BASE_LAYER) {
        run_mac_brightness_hotkey(KC_F10);
    } else {
        tap_code16(KC_BRIGHTNESS_UP);
    }
}

static void run_brightness_down_combo(void) {
    if (get_highest_layer(default_layer_state) == PLOOPY_MAC_BASE_LAYER) {
        run_mac_brightness_hotkey(KC_F11);
    } else {
        tap_code16(KC_BRIGHTNESS_DOWN);
    }
}

static void run_host_line_end_key(void) {
    uint8_t base_layer = get_highest_layer(default_layer_state);

    tap_code16(base_layer == PLOOPY_MAC_BASE_LAYER ? LGUI(KC_RIGHT) : KC_END);
}

static void run_host_screenshot_key(void) {
    uint8_t base_layer = get_highest_layer(default_layer_state);

    tap_code16(base_layer == PLOOPY_MAC_BASE_LAYER ? MAC_SCREENSHOT_KEY
                                                   : WINDOWS_SCREENSHOT_KEY);
}

static void run_double_click(void) {
    tap_code(KC_BTN1);
    wait_ms(50);
    tap_code(KC_BTN1);
}

/* ---------------------------------------------------------------------------
 * Custom keycode processing
 * ------------------------------------------------------------------------- */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!mouse_layer_process_record(keycode, record)) {
        return false;
    }

    if (behavior_raw_drag_scroll_process_record(keycode, record) ||
        behavior_raw_temporary_dpi_process_record(keycode, record)) {
        return false;
    }

    if (keycode >= Macro_M00_Outline_CollapseAll && keycode <= Macro_M13) {
        if (record->event.pressed) {
            run_defined_macro(keycode);
        }
        return false;
    }

    switch (keycode) {
        case HOST_LINE_END_KEY:
            if (record->event.pressed) {
                run_host_line_end_key();
            }
            return false;
        case HOST_SCREENSHOT_KEY:
            if (record->event.pressed) {
                run_host_screenshot_key();
            }
            return false;
        case COMBO_BRIGHTNESS_UP:
            if (record->event.pressed) {
                run_brightness_up_combo();
            }
            return false;
        case COMBO_BRIGHTNESS_DOWN:
            if (record->event.pressed) {
                run_brightness_down_combo();
            }
            return false;
        case LCLKx2:
            if (record->event.pressed) {
                run_double_click();
            }
            return false;
        default:
            return true;
    }
}


/* ---------------------------------------------------------------------------
 * Drag-scroll initialization and task
 * ------------------------------------------------------------------------- */

void keyboard_post_init_user(void) {

    drag_scroll_init();
    td_accent_init();
    if (is_keyboard_master()) {
        install_zmk_combo_defaults();
    }

}


void housekeeping_task_user(void) {

    drag_scroll_task();
    auto_mouse_layer_task();

}


/*
===========================================================================
Vial can override this compiled default layout using its EEPROM keymap.
 ===========================================================================
*/
#ifdef LAYOUT_split_3x6_3_ex2

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] =
{
  [0] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_Q,  KC_W,  KC_F,  KC_P,   KC_B,   KC_NO,                KC_NO,     KC_J,   KC_L,                      KC_U,      KC_Y,    RALT(KC_RBRC),  KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_A,  KC_R,  KC_S,  KC_T,   KC_G,   KC_NO,                KC_NO,     KC_M,   KpOrHoldMouseLayer_mac_N,  KC_E,      KC_I,    KC_O,           KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_Z,  KC_X,  KC_C,  KC_D,   KC_V,                                    KC_K,   KC_H,                      KC_COMMA,  KC_DOT,  KC_SLASH,       KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
                                      MO(8),  MO(1),  OSM(MOD_LSFT),        KC_SPACE,  MO(2),  MO(9)
      //                             +-------+-------+---------------+     +----------+-------+--------------------------+
  ),
  [1] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+-------------------+---------------------+-------+
         KC_NO,  LGUI(KC_Z),     LGUI(KC_X),     LGUI(KC_C),     LGUI(KC_V),     S(LGUI(KC_Z)),  KC_NO,        KC_NO,     LGUI(KC_LEFT),  KC_PGDN,        KC_PGUP,  HOST_LINE_END_KEY,  HOST_SCREENSHOT_KEY,  KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+-------------------+---------------------+-------+
         KC_NO,  OSM(MOD_LALT),  OSM(MOD_LGUI),  OSM(MOD_LSFT),  OSM(MOD_LCTL),  KC_BSPC,        KC_NO,        KC_NO,     KC_LEFT,        KC_DOWN,        KC_UP,    KC_RIGHT,           KC_ESCAPE,            KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+-------------------+---------------------+-------+
         KC_NO,  KC_ENTER,       LSFT(KC_TAB),   KC_TAB,         OSM(MOD_RCTL),  KC_NO,                                   KC_DELETE,      KC_BSPC,        KC_TAB,   KC_INSERT,          KC_CAPS_LOCK,         KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+-------------------+---------------------+-------+
                                                                 MO(11),         KC_NO,          KC_NO,        KC_ENTER,  LCTL(KC_LEFT),  LCTL(KC_RIGHT)
      //                                                        +---------------+---------------+-------+     +----------+---------------+---------------+
  ),
  [2] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  LSFT(KC_1),     LSFT(KC_2),  LSFT(KC_3),  LSFT(KC_4),  LSFT(KC_5),  KC_NO,                KC_NO,  LSFT(KC_6),      LSFT(KC_7),      LSFT(KC_8),      LSFT(KC_EQUAL),  RALT(KC_MINUS),  KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  KC_BSLS,        RALT(KC_7),  TD(0),       LSFT(KC_9),  RALT(KC_9),  KC_NO,                KC_NO,  KC_KP_SLASH,     KC_MINUS,        RALT(KC_COMMA),  KC_COLON,        KC_EQUAL,        KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  RALT(KC_LBRC),  RALT(KC_8),  KC_RBRC,     LSFT(KC_0),  RALT(KC_0),                                RALT(KC_GRAVE),  LSFT(KC_MINUS),  RALT(KC_DOT),    KC_SCLN,         KC_QUOTE,        KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
                                                           KC_NO,       KC_NO,       OSM(MOD_LSFT),        KC_NO,  KC_NO,           MO(10)
      //                                                  +------------+------------+---------------+     +-------+----------------+----------------+
  ),
  [3] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+------------+--------------+------------+------------+---------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  LGUI(KC_Z),  LGUI(KC_X),    LGUI(KC_C),  LGUI(KC_V),  S(LGUI(KC_Z)),  KC_NO,                 KC_NO,  KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+---------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  KC_LALT,     KC_LGUI,       KC_LSFT,     KC_LCTL,     KC_BSPC,        KC_NO,                 KC_NO,  KC_NO,          HoldMouseLayer_mac,  LatchMouseLayer_mac,  UnlatchMouseLayer_mac,  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+---------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  KC_ENTER,    LSFT(KC_TAB),  KC_TAB,      HID_TempDPI, KC_NO,                                         KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+---------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
                                                          KC_BTN2,     KC_BTN1,        HID_DragScroll,        KC_NO,  LGUI(KC_LBRC),  LGUI(KC_RBRC)
      //                                                 +------------+---------------+----------------+     +-------+---------------+--------------------+
  ),
  [4] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_Q,  KC_W,  KC_F,  KC_P,   KC_B,   KC_NO,                KC_NO,     KC_J,   KC_L,                      KC_U,      KC_Y,    RCTL(KC_RBRC),  KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_A,  KC_R,  KC_S,  KC_T,   KC_G,   KC_NO,                KC_NO,     KC_M,   KpOrHoldMouseLayer_win_N,  KC_E,      KC_I,    KC_O,           KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
         KC_NO,  KC_Z,  KC_X,  KC_C,  KC_D,   KC_V,                                    KC_K,   KC_H,                      KC_COMMA,  KC_DOT,  KC_SLASH,       KC_NO,
      //+-------+------+------+------+-------+-------+---------------+     +----------+-------+--------------------------+----------+--------+---------------+-------+
                                      MO(8),  MO(5),  OSM(MOD_LSFT),        KC_SPACE,  MO(6),  MO(9)
      //                             +-------+-------+---------------+     +----------+-------+--------------------------+
  ),

  [5] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-------------------+---------------------+-------+
         KC_NO,  LCTL(KC_Z),     LCTL(KC_X),     LCTL(KC_C),     LCTL(KC_V),     LCTL(KC_Y),  KC_NO,        KC_NO,     KC_HOME,              KC_PGDN,              KC_PGUP,  HOST_LINE_END_KEY,  HOST_SCREENSHOT_KEY,  KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-------------------+---------------------+-------+
         KC_NO,  OSM(MOD_LALT),  OSM(MOD_LGUI),  OSM(MOD_LSFT),  OSM(MOD_LCTL),  KC_BSPC,     KC_NO,        KC_NO,     KC_LEFT,              KC_DOWN,              KC_UP,    KC_RIGHT,           KC_ESCAPE,            KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-------------------+---------------------+-------+
         KC_NO,  KC_ENTER,       LSFT(KC_TAB),   KC_TAB,         OSM(MOD_RCTL),  KC_NO,                                KC_DELETE,            KC_BSPC,              KC_TAB,   KC_INSERT,          KC_CAPS_LOCK,         KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-------------------+---------------------+-------+
                                                                 MO(11),         KC_NO,       KC_NO,        KC_ENTER,  LCTL(LGUI(KC_LEFT)),  LCTL(LGUI(KC_RIGHT))
      //                                                        +---------------+------------+-------+     +----------+---------------------+---------------------+
  ),
  [6] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  LSFT(KC_1),     LSFT(KC_2),  LSFT(KC_3),  LSFT(KC_4),  LSFT(KC_5),  KC_NO,                KC_NO,  LSFT(KC_6),      LSFT(KC_7),      LSFT(KC_8),      LSFT(KC_EQUAL),  RALT(KC_GRAVE),  KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  KC_BSLS,        RALT(KC_7),  TD(0),       LSFT(KC_9),  RALT(KC_9),  KC_NO,                KC_NO,  KC_KP_SLASH,     KC_MINUS,        RALT(KC_COMMA),  KC_COLON,        KC_EQUAL,        KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
         KC_NO,  RALT(KC_LBRC),  RALT(KC_8),  KC_RBRC,     LSFT(KC_0),  RALT(KC_0),                                LSFT(KC_GRAVE),  LSFT(KC_MINUS),  RALT(KC_DOT),    KC_SCLN,         KC_QUOTE,        KC_NO,
      //+-------+---------------+------------+------------+------------+------------+---------------+     +-------+----------------+----------------+----------------+----------------+----------------+-------+
                                                           KC_NO,       KC_NO,       OSM(MOD_LSFT),        KC_NO,  KC_NO,           MO(10)
      //                                                  +------------+------------+---------------+     +-------+----------------+----------------+
  ),
  [7] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+------------+--------------+------------+------------+-------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  LCTL(KC_Z),  LCTL(KC_X),    LCTL(KC_C),  LCTL(KC_V),  LCTL(KC_Y),   KC_NO,                 KC_NO,  KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+-------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  KC_LALT,     KC_LGUI,       KC_LSFT,     KC_LCTL,     KC_BSPC,      KC_NO,                 KC_NO,  KC_NO,          HoldMouseLayer_win,  LatchMouseLayer_win,  UnlatchMouseLayer_win,  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+-------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
         KC_NO,  KC_ENTER,    LSFT(KC_TAB),  KC_TAB,      LED_TempDPI, KC_NO,                                       KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
      //+-------+------------+--------------+------------+------------+-------------+----------------+     +-------+---------------+--------------------+---------------------+-----------------------+-------+-------+
                                                          KC_BTN2,     KC_BTN1,      LED_DragScroll,        KC_NO,  LALT(KC_LEFT),  LALT(KC_RIGHT)
      //                                                 +------------+-------------+----------------+     +-------+---------------+--------------------+
  ),
  [8] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------+------------+------------------+---------+---------+-------+     +-------------+-------------+----------+---------+---------+----------------+-------+
         KC_NO,  KC_NO,    KC_NO,       C(S(A(KC_LGUI))),  KC_NO,    KC_NO,    KC_NO,        KC_NO,        KC_NO,        KC_KP_7,   KC_KP_8,  KC_KP_9,  KC_NUM_LOCK,     KC_NO,
      //+-------+---------+------------+------------------+---------+---------+-------+     +-------------+-------------+----------+---------+---------+----------------+-------+
         KC_NO,  KC_LALT,  KC_LGUI,     KC_LSFT,           KC_LCTL,  KC_BSPC,  KC_NO,        KC_NO,        KC_KP_MINUS,  KC_KP_4,   KC_KP_5,  KC_KP_6,  KC_KP_SLASH,     KC_NO,
      //+-------+---------+------------+------------------+---------+---------+-------+     +-------------+-------------+----------+---------+---------+----------------+-------+
         KC_NO,  KC_RALT,  AppleGlobe,  KC_NO,             KC_RCTL,  KC_NO,                                KC_KP_PLUS,   KC_KP_1,   KC_KP_2,  KC_KP_3,  KC_KP_ASTERISK,  KC_NO,
      //+-------+---------+------------+------------------+---------+---------+-------+     +-------------+-------------+----------+---------+---------+----------------+-------+
                                                           KC_NO,    MO(11),   KC_NO,        KC_KP_ENTER,  KC_KP_0,      KC_KP_DOT
      //                                                  +---------+---------+-------+     +-------------+-------------+----------+
  ),
  [9] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------+------------+---------+---------+-------+-------+     +-------+--------+-------+-------+-------+--------+-------+
         KC_NO,  KC_NO,    KC_NO,       KC_NO,    KC_NO,    KC_NO,  KC_NO,        KC_NO,  KC_NO,   KC_F7,  KC_F8,  KC_F9,  KC_F10,  KC_NO,
      //+-------+---------+------------+---------+---------+-------+-------+     +-------+--------+-------+-------+-------+--------+-------+
         KC_NO,  KC_LALT,  KC_LGUI,     KC_LSFT,  KC_LCTL,  KC_NO,  KC_NO,        KC_NO,  KC_NO,   KC_F4,  KC_F5,  KC_F6,  KC_F11,  KC_NO,
      //+-------+---------+------------+---------+---------+-------+-------+     +-------+--------+-------+-------+-------+--------+-------+
         KC_NO,  KC_RALT,  AppleGlobe,  KC_NO,    KC_RCTL,  KC_NO,                        KC_NO,   KC_F1,  KC_F2,  KC_F3,  KC_F12,  KC_NO,
      //+-------+---------+------------+---------+---------+-------+-------+     +-------+--------+-------+-------+-------+--------+-------+
                                                  KC_NO,    KC_NO,  KC_NO,        KC_NO,  MO(10),  KC_NO
      //                                         +---------+-------+-------+     +-------+--------+-------+
  ),
  [10] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+-------+-------+--------------+--------------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  KC_NO,         KC_NO,         KC_NO,  KC_NO,        KC_NO,  RGB_M_P,   RGB_M_B,  RGB_M_R,  RGB_M_SN,  RGB_M_K,  KC_NO,
      //+-------+-------+-------+--------------+--------------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  Macro_SetWin,  Macro_SetMac,  KC_NO,  KC_NO,        KC_NO,  RGB_MOD,   RGB_VAI,  RGB_HUI,  RGB_SAI,   RGB_SPI,  KC_NO,
      //+-------+-------+-------+--------------+--------------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  KC_NO,         KC_NO,         KC_NO,                        RGB_RMOD,  RGB_VAD,  RGB_HUD,  RGB_SAD,   RGB_SPD,  KC_NO,
      //+-------+-------+-------+--------------+--------------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
                                                KC_NO,         KC_NO,  KC_NO,        KC_NO,  KC_NO,     KC_NO
      //                                       +--------------+-------+-------+     +-------+----------+---------+
  ),
  [11] = LAYOUT_split_3x6_3_ex2
  (
      //+-------+---------+-------+-------+-------+-------+-------+     +-------+-------+-------+-------+-------+-------+-------+
         KC_NO,  QK_BOOT,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,        KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
      //+-------+---------+-------+-------+-------+-------+-------+     +-------+-------+-------+-------+-------+-------+-------+
         KC_NO,  KC_NO,    KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,        KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
      //+-------+---------+-------+-------+-------+-------+-------+     +-------+-------+-------+-------+-------+-------+-------+
         KC_NO,  KC_NO,    KC_NO,  KC_NO,  KC_NO,  KC_NO,                        KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
      //+-------+---------+-------+-------+-------+-------+-------+     +-------+-------+-------+-------+-------+-------+-------+
                                           KC_NO,  KC_NO,  KC_NO,        KC_NO,  KC_NO,  KC_NO
      //                                  +-------+-------+-------+     +-------+-------+-------+
  )
};


#else

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_split_3x6_3(

        /* Row 0 — LEFT */
        QK_BOOT, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,

        /* Row 0 — RIGHT */
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,


        /* Row 1 — LEFT */
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,

        /* Row 1 — RIGHT */
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,


        /* Row 2 — LEFT */
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,

        /* Row 2 — RIGHT */
        KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,


        /* LEFT THUMB: 3 keys */
        KC_NO, KC_NO, KC_NO,

        /* RIGHT THUMB: 3 keys */
        KC_NO, KC_NO, KC_NO

    )

};


#endif

#ifdef ENCODER_MAP_ENABLE

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

    [0] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [1] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [2] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [3] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [4] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [5] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [6] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [7] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [8] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [9] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

    [10] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },
    [11] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    },

};

#endif
