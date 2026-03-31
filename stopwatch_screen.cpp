#include "stopwatch_screen.h"
#include <lvgl.h>
#include <Arduino.h>

#define CLR_GREEN_HI  lv_color_make(0, 255,  65)
#define CLR_GREEN_DIM lv_color_make(0, 160,  40)
#define CLR_GREEN_DBG lv_color_make(0,  60,  15)

static lv_obj_t *time_lbl    = NULL;
static lv_obj_t *start_btn   = NULL;
static lv_obj_t *start_lbl   = NULL;

static uint32_t sw_elapsed_ms = 0;
static uint32_t sw_start_ms   = 0;
static bool     sw_running    = false;

static lv_timer_t *sw_timer = NULL;

static uint32_t get_elapsed(void) {
    if (sw_running) return sw_elapsed_ms + (millis() - sw_start_ms);
    return sw_elapsed_ms;
}

static void update_display(void) {
    if (!time_lbl) return;
    uint32_t total = get_elapsed();
    uint32_t mins  = total / 60000;
    uint32_t secs  = (total % 60000) / 1000;
    uint32_t tenth = (total % 1000) / 100;
    lv_label_set_text_fmt(time_lbl, "%02lu:%02lu.%lu", mins, secs, tenth);
}

static void sw_timer_cb(lv_timer_t *t) {
    (void)t;
    if (sw_running) update_display();
}

static void start_btn_cb(lv_event_t *e) {
    (void)e;
    if (sw_running) {
        // Pause
        sw_elapsed_ms += millis() - sw_start_ms;
        sw_running = false;
        lv_label_set_text(start_lbl, "START");
        lv_obj_set_style_border_color(start_btn, CLR_GREEN_DIM, 0);
        lv_obj_set_style_text_color(start_lbl, CLR_GREEN_DIM, 0);
    } else {
        // Start
        sw_start_ms = millis();
        sw_running  = true;
        lv_label_set_text(start_lbl, "PAUSE");
        lv_obj_set_style_border_color(start_btn, CLR_GREEN_HI, 0);
        lv_obj_set_style_text_color(start_lbl, CLR_GREEN_HI, 0);
    }
}

static void reset_btn_cb(lv_event_t *e) {
    (void)e;
    sw_elapsed_ms = 0;
    sw_running    = false;
    sw_start_ms   = 0;
    lv_label_set_text(start_lbl, "START");
    lv_obj_set_style_border_color(start_btn, CLR_GREEN_DIM, 0);
    lv_obj_set_style_text_color(start_lbl, CLR_GREEN_DIM, 0);
    update_display();
}

void stopwatch_handle_tap(int raw_x, int raw_y) {
    static uint32_t last_tap_ms = 0;
    uint32_t now = millis();
    if (now - last_tap_ms < 300) return;
    last_tap_ms = now;

    // Convert raw CST816 coords to LVGL coords (180° rotation)
    int lx = 359 - raw_x;
    int ly = 359 - raw_y;

    // START/PAUSE button: LVGL center(180,200), size 160x50 → x:100-260, y:175-225
    if (lx >= 100 && lx <= 260 && ly >= 175 && ly <= 225) {
        if (sw_running) {
            sw_elapsed_ms += millis() - sw_start_ms;
            sw_running = false;
            lv_label_set_text(start_lbl, "START");
            lv_obj_set_style_border_color(start_btn, CLR_GREEN_DIM, 0);
            lv_obj_set_style_text_color(start_lbl, CLR_GREEN_DIM, 0);
        } else {
            sw_start_ms = millis();
            sw_running  = true;
            lv_label_set_text(start_lbl, "PAUSE");
            lv_obj_set_style_border_color(start_btn, CLR_GREEN_HI, 0);
            lv_obj_set_style_text_color(start_lbl, CLR_GREEN_HI, 0);
        }
        return;
    }

    // RESET button: LVGL center(180,260), size 100x36 → x:130-230, y:242-278
    if (lx >= 130 && lx <= 230 && ly >= 242 && ly <= 278) {
        sw_elapsed_ms = 0;
        sw_running    = false;
        sw_start_ms   = 0;
        lv_label_set_text(start_lbl, "START");
        lv_obj_set_style_border_color(start_btn, CLR_GREEN_DIM, 0);
        lv_obj_set_style_text_color(start_lbl, CLR_GREEN_DIM, 0);
        update_display();
    }
}

lv_obj_t * stopwatch_screen_create(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360, 360);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);

    // ── Title ────────────────────────────────────────────────
    static lv_style_t title_sty;
    lv_style_init(&title_sty);
    lv_style_set_text_font(&title_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&title_sty, CLR_GREEN_HI);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_add_style(title, &title_sty, 0);
    lv_label_set_text(title, "> STOPWATCH <");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 38);

    // ── Divider ──────────────────────────────────────────────
    lv_obj_t *div = lv_obj_create(scr);
    lv_obj_set_size(div, 220, 1);
    lv_obj_set_style_bg_color(div, CLR_GREEN_DBG, 0);
    lv_obj_set_style_bg_opa(div, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div, 0, 0);
    lv_obj_set_style_pad_all(div, 0, 0);
    lv_obj_align(div, LV_ALIGN_TOP_MID, 0, 68);

    // ── Timer display ────────────────────────────────────────
    static lv_style_t time_sty;
    lv_style_init(&time_sty);
    lv_style_set_text_font(&time_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&time_sty, CLR_GREEN_HI);

    time_lbl = lv_label_create(scr);
    lv_obj_add_style(time_lbl, &time_sty, 0);
    lv_label_set_text(time_lbl, "00:00.0");
    lv_obj_align(time_lbl, LV_ALIGN_CENTER, 0, -40);

    // ── START/PAUSE button ───────────────────────────────────
    start_btn = lv_btn_create(scr);
    lv_obj_set_size(start_btn, 160, 50);
    lv_obj_align(start_btn, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(start_btn, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(start_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(start_btn, CLR_GREEN_DIM, 0);
    lv_obj_set_style_border_width(start_btn, 2, 0);
    lv_obj_set_style_radius(start_btn, 0, 0);
    lv_obj_clear_flag(start_btn, LV_OBJ_FLAG_CLICKABLE);

    start_lbl = lv_label_create(start_btn);
    lv_obj_set_style_text_font(start_lbl, &lv_font_unscii_16, 0);
    lv_obj_set_style_text_color(start_lbl, CLR_GREEN_DIM, 0);
    lv_label_set_text(start_lbl, "START");
    lv_obj_center(start_lbl);

    // ── RESET button ─────────────────────────────────────────
    lv_obj_t *reset_btn = lv_btn_create(scr);
    lv_obj_set_size(reset_btn, 100, 36);
    lv_obj_align(reset_btn, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_bg_color(reset_btn, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(reset_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(reset_btn, CLR_GREEN_DBG, 0);
    lv_obj_set_style_border_width(reset_btn, 1, 0);
    lv_obj_set_style_radius(reset_btn, 0, 0);
    lv_obj_clear_flag(reset_btn, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *reset_lbl = lv_label_create(reset_btn);
    lv_obj_set_style_text_font(reset_lbl, &lv_font_unscii_8, 0);
    lv_obj_set_style_text_color(reset_lbl, CLR_GREEN_DBG, 0);
    lv_label_set_text(reset_lbl, "RESET");
    lv_obj_center(reset_lbl);

    // ── LVGL timer for display refresh ───────────────────────
    if (!sw_timer) {
        sw_timer = lv_timer_create(sw_timer_cb, 100, NULL);
    }

    update_display();
    return scr;
}
