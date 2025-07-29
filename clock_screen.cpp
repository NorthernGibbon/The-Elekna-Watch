#include "clock_screen.h"
#include "rtc_driver.h"
#include <lvgl.h>

// Needle objects and data
static lv_obj_t *scale;
static lv_obj_t *minute_hand;
static lv_obj_t *hour_hand;
static lv_obj_t *seconds_hand;

static lv_point_precise_t second_hand_points[2];
static lv_point_precise_t minute_hand_points[2];
static lv_point_precise_t hour_hand_points[2];

static datetime_t rtc_time;

static void timer_cb(lv_timer_t * timer) {
    LV_UNUSED(timer);
    RTC_GetDateTime(&rtc_time);
    int s = rtc_time.second;
    int m = rtc_time.minute;
    int h = rtc_time.hour % 12;

    lv_scale_set_line_needle_value(scale, seconds_hand, 100, rtc_time.second);
    lv_scale_set_line_needle_value(scale, minute_hand,  80, rtc_time.minute);
    lv_scale_set_line_needle_value(scale, hour_hand, 60, (rtc_time.hour % 12) * 5 + (rtc_time.minute / 12)
  );
}

lv_obj_t * clock_screen_create(void) {
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360,360);
    lv_obj_center(scr);

    scale = lv_scale_create(scr);
    lv_obj_set_size(scale, 360,360);
    lv_obj_center(scale);

    // ← Add this so the scale knows your value range is 0…60:
    lv_scale_set_range(scale, 0, 60);

    // Round‐inner 360° dial, starting at “12”
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_rotation(scale, 270);
    lv_obj_set_style_bg_opa(scale, LV_OPA_60, 0);
    lv_obj_set_style_bg_color(scale, lv_color_black(), 0);

    // Ticks & labels
    lv_scale_set_total_tick_count(scale, 61);
    lv_scale_set_major_tick_every(scale, 5);
    lv_scale_set_label_show(scale, true);
    static const char *ticks[] = {
        "12","1","2","3","4","5","6","7","8","9","10","11", NULL
    };
    lv_scale_set_text_src(scale, ticks);
    
    static lv_style_t maj;
    lv_style_init(&maj);
    lv_style_set_line_color(&maj, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_length(&maj,12);
    lv_style_set_line_width(&maj,3);
    lv_obj_add_style(scale,&maj,LV_PART_INDICATOR);
    
    static lv_style_t min;
    lv_style_init(&min);
    lv_style_set_line_color(&min, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_length(&min,10);
    lv_style_set_line_width(&min,2);
    lv_obj_add_style(scale,&min,LV_PART_ITEMS);

    // Seconds hand
    seconds_hand = lv_line_create(scale);
    lv_line_set_points_mutable(seconds_hand, second_hand_points, 2);
    lv_obj_set_style_line_width(seconds_hand, 2,   0);
    lv_obj_set_style_line_rounded(seconds_hand, true, 0);
    lv_obj_set_style_line_color(seconds_hand, lv_palette_main(LV_PALETTE_GREEN), 0);

    // Minute hand
    minute_hand = lv_line_create(scale);
    lv_line_set_points_mutable(minute_hand, minute_hand_points, 2);
    lv_obj_set_style_line_width(minute_hand, 4,   0);
    lv_obj_set_style_line_rounded(minute_hand, true, 0);
    lv_obj_set_style_line_color(minute_hand, lv_palette_main(LV_PALETTE_BLUE), 0);

    // Hour hand
    hour_hand = lv_line_create(scale);
    lv_line_set_points_mutable(hour_hand, hour_hand_points, 2);
    lv_obj_set_style_line_width(hour_hand, 6,    0);
    lv_obj_set_style_line_rounded(hour_hand, true, 0);
    lv_obj_set_style_line_color(hour_hand, lv_palette_main(LV_PALETTE_RED), 0);

    // 1 Hz update timer
    lv_timer_t *tm = lv_timer_create(timer_cb, 250, NULL);
    lv_timer_ready(tm);

    return scr;
}