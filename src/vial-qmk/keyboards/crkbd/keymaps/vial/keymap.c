#include QMK_KEYBOARD_H

#include "modules/Ploopy/behavior_mouse_layer.h"
#include "modules/tap_dance/tap_dance.h"


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

    switch (keycode)
    {
        case LCLKx2:

            if (record->event.pressed) {
                tap_code(KC_BTN1);
                wait_ms(50);
                tap_code(KC_BTN1);
            }

            return false;

    }

    return true;
}


/* ---------------------------------------------------------------------------
 * Drag-scroll initialization and task
 * ------------------------------------------------------------------------- */

void keyboard_post_init_user(void) {

    drag_scroll_init();
    td_accent_init();

}


void housekeeping_task_user(void) {

    drag_scroll_task();
    auto_mouse_layer_task();

}


/* ===========================================================================
 * KEYMAP
 *
 * Layer key assignments in layers 0-9 follow the shared ZMK Corne keymap;
 * ZMK combos and macros are not ported. QMK-specific layers 10 and 11 are
 * retained. The ex2 layout has four extra top/home positions; they are KC_NO.
 * Rows are spaced by physical x-column; visual gaps are not macro arguments.
 *
 * Vial can override this compiled default layout using its EEPROM keymap.
 * =========================================================================== */


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
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+----------------+---------------+-------+
         KC_NO,  LGUI(KC_Z),     LGUI(KC_X),     LGUI(KC_C),     LGUI(KC_V),     S(LGUI(KC_Z)),  KC_NO,        KC_NO,     LGUI(KC_LEFT),  KC_PGDN,        KC_PGUP,  LGUI(KC_RIGHT),  LGUI(S(KC_4)),  KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+----------------+---------------+-------+
         KC_NO,  OSM(MOD_LALT),  OSM(MOD_LGUI),  OSM(MOD_LSFT),  OSM(MOD_LCTL),  KC_BSPC,        KC_NO,        KC_NO,     KC_LEFT,        KC_DOWN,        KC_UP,    KC_RIGHT,        KC_ESCAPE,      KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+----------------+---------------+-------+
         KC_NO,  KC_ENTER,       LSFT(KC_TAB),   KC_TAB,         OSM(MOD_RCTL),  KC_NO,                                   KC_DELETE,      KC_BSPC,        KC_TAB,   KC_INSERT,       KC_CAPS_LOCK,   KC_NO,
      //+-------+---------------+---------------+---------------+---------------+---------------+-------+     +----------+---------------+---------------+---------+----------------+---------------+-------+
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
         KC_NO,  KC_ENTER,    LSFT(KC_TAB),  KC_TAB,      KC_TAB,      HID_TempDPI,                                   KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
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
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-----------+--------------+-------+
         KC_NO,  LCTL(KC_Z),     LCTL(KC_X),     LCTL(KC_C),     LCTL(KC_V),     LCTL(KC_Y),  KC_NO,        KC_NO,     KC_HOME,              KC_PGDN,              KC_PGUP,  KC_END,     KC_PSCR,       KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-----------+--------------+-------+
         KC_NO,  OSM(MOD_LALT),  OSM(MOD_LGUI),  OSM(MOD_LSFT),  OSM(MOD_LCTL),  KC_BSPC,     KC_NO,        KC_NO,     KC_LEFT,              KC_DOWN,              KC_UP,    KC_RIGHT,   KC_ESCAPE,     KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-----------+--------------+-------+
         KC_NO,  KC_ENTER,       LSFT(KC_TAB),   KC_TAB,         OSM(MOD_RCTL),  KC_NO,                                KC_DELETE,            KC_BSPC,              KC_TAB,   KC_INSERT,  KC_CAPS_LOCK,  KC_NO,
      //+-------+---------------+---------------+---------------+---------------+------------+-------+     +----------+---------------------+---------------------+---------+-----------+--------------+-------+
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
         KC_NO,  KC_ENTER,    LSFT(KC_TAB),  KC_TAB,      KC_TAB,      LED_TempDPI,                                 KC_NO,          KC_NO,               KC_NO,                KC_NO,                  KC_NO,  KC_NO,
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
      //+-------+-------+-------+-------+-------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,        KC_NO,  RGB_M_P,   RGB_M_B,  RGB_M_R,  RGB_M_SN,  RGB_M_K,  KC_NO,
      //+-------+-------+-------+-------+-------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  DF(4),  DF(0),  KC_NO,  KC_NO,        KC_NO,  RGB_MOD,   RGB_VAI,  RGB_HUI,  RGB_SAI,   RGB_SPI,  KC_NO,
      //+-------+-------+-------+-------+-------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
         KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,                        RGB_RMOD,  RGB_VAD,  RGB_HUD,  RGB_SAD,   RGB_SPD,  KC_NO,
      //+-------+-------+-------+-------+-------+-------+-------+     +-------+----------+---------+---------+----------+---------+-------+
                                         KC_NO,  KC_NO,  KC_NO,        KC_NO,  KC_NO,     KC_NO
      //                                +-------+-------+-------+     +-------+----------+---------+
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
