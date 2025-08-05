#include "clock_screen.h"
#include "rtc_driver.h"
#include <lvgl.h>

static lv_obj_t *scale;
static lv_obj_t *minute_hand;
static lv_obj_t *hour_hand;
static lv_obj_t *seconds_hand;
static lv_obj_t *circle;
static lv_obj_t *title_lbl;
static lv_obj_t *date_lbl;

static lv_point_precise_t second_hand_points[2];
static lv_point_precise_t minute_hand_points[2];
static lv_point_precise_t hour_hand_points[2];

static datetime_t rtc_time;

static const char * get_day_suffix(int d) {
    if (d % 100 >= 11 && d % 100 <= 13) return "th";
    switch (d % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }
}

void clock_screen_update(void) {
    // Time
    RTC_GetDateTime(&rtc_time);
    int s = rtc_time.second;
    int m = rtc_time.minute;
    int h = rtc_time.hour % 12;

    lv_scale_set_line_needle_value(scale, seconds_hand, 120, s);
    lv_scale_set_line_needle_value(scale, minute_hand,  100, m);
    lv_scale_set_line_needle_value(scale, hour_hand,    80, h * 5 + (m / 12));

    // Date
    static const char *day_names[] = {
      "Sunday","Monday","Tuesday","Wednesday",
      "Thursday","Friday","Saturday"
    };
    const char *day_str = day_names[rtc_time.dotw];
    int d = rtc_time.day;
    const char *suffix = get_day_suffix(d);
    lv_label_set_text_fmt(date_lbl, "%s %d%s", day_str, d, suffix);
}
/*
=========================================================
  Functionality: LVGL object creating the clock screen
=========================================================
*/
lv_obj_t * clock_screen_create(void) {
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360, 360);
    lv_obj_center(scr);

    // Create the arc for the watch
    scale = lv_scale_create(scr);
    lv_obj_set_size(scale, 360, 360);
    lv_obj_center(scale);

    lv_scale_set_range(scale, 0, 60);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_rotation(scale, 270);
    lv_obj_set_style_bg_opa(scale, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(scale, lv_color_black(), 0);
    
    // Removes the 1px grey outline // I need to use this in other screens 
    lv_obj_set_style_border_width(scale, 0, 0);
    lv_obj_set_style_outline_width(scale, 0, 0);

    lv_scale_set_label_show(scale, false);

    lv_scale_set_total_tick_count(scale, 61);
    lv_scale_set_major_tick_every(scale, 5);

    // title
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_color(&title_style, lv_color_white());
    lv_style_set_text_font(&title_style, &lv_font_montserrat_22);

    title_lbl = lv_label_create(scr);
    lv_obj_add_style(title_lbl, &title_style, LV_PART_MAIN);
    lv_label_set_text(title_lbl, "ELEKNA");
    lv_obj_align(title_lbl, LV_ALIGN_TOP_MID, 0, 40);

    // Date
    static lv_style_t date_style;
    lv_style_init(&date_style);
    lv_style_set_text_color(&date_style, lv_color_white());
    lv_style_set_text_font(&date_style, &lv_font_montserrat_18);

    date_lbl = lv_label_create(scr);
    lv_obj_add_style(date_lbl, &date_style, LV_PART_MAIN);
    lv_label_set_text(date_lbl, "");
    lv_obj_align(date_lbl, LV_ALIGN_TOP_MID, 0, 75);

    // Major tick style
    static lv_style_t maj;
    lv_style_init(&maj);
    lv_style_set_line_color(&maj, lv_color_white());
    lv_style_set_length(&maj, 18);
    lv_style_set_line_width(&maj, 3);
    lv_obj_add_style(scale, &maj, LV_PART_INDICATOR);

    // Minor tick style
    static lv_style_t min;
    lv_style_init(&min);
    lv_style_set_line_color(&min, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_length(&min, 10);
    lv_style_set_line_width(&min, 2);
    lv_obj_add_style(scale, &min, LV_PART_ITEMS);

    // Time hands
    seconds_hand = lv_line_create(scale);
    lv_line_set_points_mutable(seconds_hand, second_hand_points, 2);
    lv_obj_set_style_line_width(seconds_hand, 2, 0);
    lv_obj_set_style_line_rounded(seconds_hand, true, 0);
    lv_obj_set_style_line_color(seconds_hand, lv_color_white(), 0);

    minute_hand = lv_line_create(scale);
    lv_line_set_points_mutable(minute_hand, minute_hand_points, 2);
    lv_obj_set_style_line_width(minute_hand, 2, 0);
    lv_obj_set_style_line_rounded(minute_hand, true, 0);
    lv_obj_set_style_line_color(minute_hand, lv_color_white(), 0);

    hour_hand = lv_line_create(scale);
    lv_line_set_points_mutable(hour_hand, hour_hand_points, 2);
    lv_obj_set_style_line_width(hour_hand, 2, 0);
    lv_obj_set_style_line_rounded(hour_hand, true, 0);
    lv_obj_set_style_line_color(hour_hand, lv_color_white(), 0);

    // Create the circular center
    static lv_style_t circle_style;
    lv_style_init(&circle_style);
    lv_style_set_bg_color(&circle_style, lv_color_white());
    lv_style_set_radius(&circle_style, LV_RADIUS_CIRCLE);
    
    circle = lv_obj_create(scr);
    lv_obj_add_style(circle, &circle_style, 0);
    lv_obj_set_size(circle, 10, 10);
    lv_obj_center(circle);

    clock_screen_update();

    return scr;
}

