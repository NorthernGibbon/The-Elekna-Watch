#include "activity_screen.h"
#include "step_counter.h"
#include <lvgl.h>

#define CLR_GREEN_HI  lv_color_make(0, 255,  65)
#define CLR_GREEN_DIM lv_color_make(0, 160,  40)
#define CLR_GREEN_DBG lv_color_make(0,  60,  15)

#define STEP_GOAL 10000

static lv_obj_t *step_arc       = NULL;
static lv_obj_t *step_count_lbl = NULL;
static lv_obj_t *step_goal_lbl  = NULL;

void activity_screen_update(void) {
    if (!step_arc) return;
    uint32_t steps  = StepCounter_GetSteps();
    uint32_t capped = steps < STEP_GOAL ? steps : STEP_GOAL;
    lv_arc_set_value(step_arc, (int32_t)capped);
    lv_label_set_text_fmt(step_count_lbl, "%05lu", steps);
    int pct = (int)((steps * 100) / STEP_GOAL);
    if (pct > 100) pct = 100;
    lv_label_set_text_fmt(step_goal_lbl, "%03d%% OF GOAL", pct);
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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 38);

    // ── Step arc ─────────────────────────────────────────────
    step_arc = lv_arc_create(scr);
    lv_obj_set_size(step_arc, 210, 210);
    lv_obj_center(step_arc);
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
    lv_obj_align(step_count_lbl, LV_ALIGN_CENTER, 0, -12);

    // ── "STEPS TODAY" label ──────────────────────────────────
    static lv_style_t sub_sty;
    lv_style_init(&sub_sty);
    lv_style_set_text_font(&sub_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&sub_sty, CLR_GREEN_DIM);

    lv_obj_t *sub = lv_label_create(scr);
    lv_obj_add_style(sub, &sub_sty, 0);
    lv_label_set_text(sub, "STEPS TODAY");
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 18);

    // ── Goal % ───────────────────────────────────────────────
    static lv_style_t goal_sty;
    lv_style_init(&goal_sty);
    lv_style_set_text_font(&goal_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&goal_sty, CLR_GREEN_HI);

    step_goal_lbl = lv_label_create(scr);
    lv_obj_add_style(step_goal_lbl, &goal_sty, 0);
    lv_label_set_text(step_goal_lbl, "000% OF GOAL");
    lv_obj_align(step_goal_lbl, LV_ALIGN_BOTTOM_MID, 0, -48);

    activity_screen_update();
    return scr;
}
