#include "activity_screen.h"
#include "step_counter.h"
#include "sd_logger.h"
#include <lvgl.h>

#define CLR_GREEN_HI  lv_color_make(0, 255,  65)
#define CLR_GREEN_DIM lv_color_make(0, 160,  40)
#define CLR_GREEN_DBG lv_color_make(0,  60,  15)

#define STEP_GOAL  10000
#define HIST_DAYS  6      // past days shown in chart (+ today = 7 bars)

static lv_obj_t *step_arc       = NULL;
static lv_obj_t *step_count_lbl = NULL;
static lv_obj_t *step_goal_lbl  = NULL;
static lv_obj_t *hist_chart     = NULL;
static lv_chart_series_t *hist_ser = NULL;

void activity_screen_update(void) {
    if (!step_arc) return;
    uint32_t steps  = StepCounter_GetSteps();
    uint32_t capped = steps < STEP_GOAL ? steps : STEP_GOAL;
    lv_arc_set_value(step_arc, (int32_t)capped);
    lv_label_set_text_fmt(step_count_lbl, "%05lu", steps);
    int pct = (int)((steps * 100) / STEP_GOAL);
    if (pct > 100) pct = 100;
    lv_label_set_text_fmt(step_goal_lbl, "%d%% OF GOAL", pct);

    // Update today bar (last bar)
    if (hist_ser && hist_chart) {
        lv_chart_set_value_by_id(hist_chart, hist_ser, HIST_DAYS, (lv_value_precise_t)steps);
        lv_chart_refresh(hist_chart);
    }
}

void activity_screen_refresh_history(void) {
    if (!hist_chart || !hist_ser) return;

    uint32_t hist[HIST_DAYS] = {0};
    int got = SD_Logger_GetHistory(hist, HIST_DAYS);

    for (int i = 0; i < HIST_DAYS; i++) {
        uint32_t val = (i < HIST_DAYS - got) ? 0 : hist[i - (HIST_DAYS - got)];
        lv_chart_set_value_by_id(hist_chart, hist_ser, i, (lv_value_precise_t)val);
    }
    lv_chart_set_value_by_id(hist_chart, hist_ser, HIST_DAYS,
                             (lv_value_precise_t)StepCounter_GetSteps());
    lv_chart_refresh(hist_chart);
}

lv_obj_t * activity_screen_create(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360, 360);
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
    lv_label_set_text(title, "> ACTIVITY <");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 35);

    // ── Step arc (shifted up to leave room for chart) ────────
    step_arc = lv_arc_create(scr);
    lv_obj_set_size(step_arc, 160, 160);
    lv_obj_align(step_arc, LV_ALIGN_CENTER, 0, -42);
    lv_arc_set_range(step_arc, 0, STEP_GOAL);
    lv_arc_set_value(step_arc, 0);
    lv_arc_set_bg_angles(step_arc, 140, 40);
    lv_arc_set_angles(step_arc, 140, 140);
    lv_obj_remove_style(step_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(step_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(step_arc, CLR_GREEN_HI,  LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(step_arc, 8,             LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(step_arc, false,       LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(step_arc, CLR_GREEN_DBG, LV_PART_MAIN);
    lv_obj_set_style_arc_width(step_arc, 8,             LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(step_arc, false,       LV_PART_MAIN);
    lv_obj_set_style_bg_opa(step_arc, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(step_arc, 0, 0);

    // ── Step count ───────────────────────────────────────────
    static lv_style_t count_sty;
    lv_style_init(&count_sty);
    lv_style_set_text_font(&count_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&count_sty, CLR_GREEN_HI);

    step_count_lbl = lv_label_create(scr);
    lv_obj_add_style(step_count_lbl, &count_sty, 0);
    lv_label_set_text(step_count_lbl, "00000");
    lv_obj_align(step_count_lbl, LV_ALIGN_CENTER, 0, -52);

    // ── Sub-labels ───────────────────────────────────────────
    static lv_style_t sub_sty;
    lv_style_init(&sub_sty);
    lv_style_set_text_font(&sub_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&sub_sty, CLR_GREEN_DIM);

    lv_obj_t *sub = lv_label_create(scr);
    lv_obj_add_style(sub, &sub_sty, 0);
    lv_label_set_text(sub, "STEPS TODAY");
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, -25);

    static lv_style_t goal_sty;
    lv_style_init(&goal_sty);
    lv_style_set_text_font(&goal_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&goal_sty, CLR_GREEN_HI);

    step_goal_lbl = lv_label_create(scr);
    lv_obj_add_style(step_goal_lbl, &goal_sty, 0);
    lv_label_set_text(step_goal_lbl, "0% OF GOAL");
    lv_obj_align(step_goal_lbl, LV_ALIGN_CENTER, 0, -7);

    // ── Divider ──────────────────────────────────────────────
    lv_obj_t *div = lv_obj_create(scr);
    lv_obj_set_size(div, 220, 1);
    lv_obj_set_style_bg_color(div, CLR_GREEN_DBG, 0);
    lv_obj_set_style_bg_opa(div, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div, 0, 0);
    lv_obj_set_style_pad_all(div, 0, 0);
    lv_obj_align(div, LV_ALIGN_CENTER, 0, 13);

    // ── History label ────────────────────────────────────────
    lv_obj_t *hist_lbl = lv_label_create(scr);
    lv_obj_add_style(hist_lbl, &sub_sty, 0);
    lv_label_set_text(hist_lbl, "7 DAY HISTORY");
    lv_obj_align(hist_lbl, LV_ALIGN_CENTER, 0, 29);

    // ── Bar chart ────────────────────────────────────────────
    hist_chart = lv_chart_create(scr);
    lv_obj_set_size(hist_chart, 220, 80);
    lv_obj_align(hist_chart, LV_ALIGN_CENTER, 0, 81);
    lv_chart_set_type(hist_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(hist_chart, HIST_DAYS + 1);
    lv_chart_set_range(hist_chart, LV_CHART_AXIS_PRIMARY_Y, 0, STEP_GOAL);
    lv_chart_set_div_line_count(hist_chart, 0, 0);

    lv_obj_set_style_bg_color(hist_chart, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(hist_chart, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(hist_chart, 0, 0);
    lv_obj_set_style_pad_all(hist_chart, 2, 0);
    lv_obj_set_style_line_opa(hist_chart, LV_OPA_TRANSP, 0);

    lv_obj_set_style_bg_color(hist_chart, CLR_GREEN_DBG, LV_PART_ITEMS);
    lv_obj_set_style_radius(hist_chart, 0, LV_PART_ITEMS);

    hist_ser = lv_chart_add_series(hist_chart, CLR_GREEN_HI, LV_CHART_AXIS_PRIMARY_Y);

    activity_screen_refresh_history();
    activity_screen_update();
    return scr;
}
