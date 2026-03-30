#include "clock_screen.h"
#include "rtc_driver.h"
#include <lvgl.h>

#define CLR_GREEN_HI  lv_color_make(0, 255,  65)
#define CLR_GREEN_DIM lv_color_make(0, 160,  40)
#define CLR_GREEN_DBG lv_color_make(0,  60,  15)

static lv_obj_t *scale;
static lv_obj_t *minute_hand;
static lv_obj_t *hour_hand;
static lv_obj_t *seconds_hand;
static lv_obj_t *center_dot;
static lv_obj_t *title_lbl;
static lv_obj_t *date_lbl;

static lv_point_precise_t second_hand_pts[2];
static lv_point_precise_t minute_hand_pts[2];
static lv_point_precise_t hour_hand_pts[2];

static datetime_t rtc_time;

void clock_screen_update(void) {
    RTC_GetDateTime(&rtc_time);
    int s = rtc_time.second;
    int m = rtc_time.minute;
    int h = rtc_time.hour % 12;

    lv_scale_set_line_needle_value(scale, seconds_hand, 128, s);
    lv_scale_set_line_needle_value(scale, minute_hand,  108, m);
    lv_scale_set_line_needle_value(scale, hour_hand,     80, h * 5 + (m / 12));

    static const char *days[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
    static const char *months[] = {"JAN","FEB","MAR","APR","MAY","JUN",
                                   "JUL","AUG","SEP","OCT","NOV","DEC"};
    lv_label_set_text_fmt(date_lbl, "%s  %02d %s",
        days[rtc_time.dotw], rtc_time.day, months[rtc_time.month - 1]);
}

lv_obj_t * clock_screen_create(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360, 360);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);

    // ── Scale / tick ring ────────────────────────────────────
    scale = lv_scale_create(scr);
    lv_obj_set_size(scale, 360, 360);
    lv_obj_center(scale);
    lv_scale_set_range(scale, 0, 60);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_rotation(scale, 270);
    lv_scale_set_label_show(scale, false);
    lv_scale_set_total_tick_count(scale, 61);
    lv_scale_set_major_tick_every(scale, 5);
    lv_obj_set_style_bg_opa(scale, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scale, 0, 0);
    lv_obj_set_style_outline_width(scale, 0, 0);

    // Major ticks — bright green, square
    static lv_style_t maj_sty;
    lv_style_init(&maj_sty);
    lv_style_set_line_color(&maj_sty, CLR_GREEN_HI);
    lv_style_set_length(&maj_sty, 16);
    lv_style_set_line_width(&maj_sty, 3);
    lv_style_set_line_rounded(&maj_sty, false);
    lv_obj_add_style(scale, &maj_sty, LV_PART_INDICATOR);

    // Minor ticks — dim green, thin
    static lv_style_t min_sty;
    lv_style_init(&min_sty);
    lv_style_set_line_color(&min_sty, CLR_GREEN_DBG);
    lv_style_set_length(&min_sty, 8);
    lv_style_set_line_width(&min_sty, 1);
    lv_style_set_line_rounded(&min_sty, false);
    lv_obj_add_style(scale, &min_sty, LV_PART_ITEMS);

    // ── Hands ────────────────────────────────────────────────
    // Seconds — bright green, thin
    seconds_hand = lv_line_create(scale);
    lv_line_set_points_mutable(seconds_hand, second_hand_pts, 2);
    lv_obj_set_style_line_width(seconds_hand, 1, 0);
    lv_obj_set_style_line_rounded(seconds_hand, false, 0);
    lv_obj_set_style_line_color(seconds_hand, CLR_GREEN_HI, 0);

    // Minutes — green, medium
    minute_hand = lv_line_create(scale);
    lv_line_set_points_mutable(minute_hand, minute_hand_pts, 2);
    lv_obj_set_style_line_width(minute_hand, 3, 0);
    lv_obj_set_style_line_rounded(minute_hand, false, 0);
    lv_obj_set_style_line_color(minute_hand, CLR_GREEN_DIM, 0);

    // Hours — dim green, thick
    hour_hand = lv_line_create(scale);
    lv_line_set_points_mutable(hour_hand, hour_hand_pts, 2);
    lv_obj_set_style_line_width(hour_hand, 4, 0);
    lv_obj_set_style_line_rounded(hour_hand, false, 0);
    lv_obj_set_style_line_color(hour_hand, CLR_GREEN_DIM, 0);

    // ── Center dot ───────────────────────────────────────────
    center_dot = lv_obj_create(scr);
    lv_obj_set_size(center_dot, 8, 8);
    lv_obj_set_style_bg_color(center_dot, CLR_GREEN_HI, 0);
    lv_obj_set_style_bg_opa(center_dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(center_dot, 0, 0);
    lv_obj_set_style_radius(center_dot, 0, 0);
    lv_obj_center(center_dot);

    // ── Title ────────────────────────────────────────────────
    static lv_style_t title_sty;
    lv_style_init(&title_sty);
    lv_style_set_text_font(&title_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&title_sty, CLR_GREEN_HI);

    title_lbl = lv_label_create(scr);
    lv_obj_add_style(title_lbl, &title_sty, LV_PART_MAIN);
    lv_label_set_text(title_lbl, "ELEKNA");
    lv_obj_align(title_lbl, LV_ALIGN_TOP_MID, 0, 48);

    // ── Date ─────────────────────────────────────────────────
    static lv_style_t date_sty;
    lv_style_init(&date_sty);
    lv_style_set_text_font(&date_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&date_sty, CLR_GREEN_DIM);

    date_lbl = lv_label_create(scr);
    lv_obj_add_style(date_lbl, &date_sty, LV_PART_MAIN);
    lv_label_set_text(date_lbl, "");
    lv_obj_align(date_lbl, LV_ALIGN_TOP_MID, 0, 76);

    clock_screen_update();
    return scr;
}
