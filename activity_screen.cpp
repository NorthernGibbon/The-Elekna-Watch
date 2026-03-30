#include "activity_screen.h"
#include "step_counter.h"
#include <lvgl.h>

#define STEP_GOAL 10000

static lv_obj_t *step_arc    = NULL;
static lv_obj_t *step_count_lbl = NULL;
static lv_obj_t *step_goal_lbl  = NULL;

void activity_screen_update(void) {
    if (!step_arc) return;

    uint32_t steps = StepCounter_GetSteps();
    uint32_t capped = steps < STEP_GOAL ? steps : STEP_GOAL;

    lv_arc_set_value(step_arc, (int32_t)capped);
    lv_label_set_text_fmt(step_count_lbl, "%lu", steps);

    int pct = (int)((steps * 100) / STEP_GOAL);
    if (pct > 100) pct = 100;
    lv_label_set_text_fmt(step_goal_lbl, "%d%% of goal", pct);
}

lv_obj_t * activity_screen_create(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_size(scr, 360, 360);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);

    // Title
    static lv_style_t title_style;
    lv_style_init(&title_style);
    lv_style_set_text_color(&title_style, lv_color_white());
    lv_style_set_text_font(&title_style, &lv_font_montserrat_22);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_add_style(title, &title_style, 0);
    lv_label_set_text(title, "ACTIVITY");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);

    // Step progress arc
    step_arc = lv_arc_create(scr);
    lv_obj_set_size(step_arc, 220, 220);
    lv_obj_center(step_arc);
    lv_arc_set_range(step_arc, 0, STEP_GOAL);
    lv_arc_set_value(step_arc, 0);
    lv_arc_set_bg_angles(step_arc, 135, 45);
    lv_arc_set_angles(step_arc, 135, 135);
    lv_obj_remove_style(step_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(step_arc, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_color(step_arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(step_arc, 12, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(step_arc, lv_color_make(40, 40, 40), LV_PART_MAIN);
    lv_obj_set_style_arc_width(step_arc, 12, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(step_arc, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(step_arc, 0, 0);

    // Large step count number
    static lv_style_t count_style;
    lv_style_init(&count_style);
    lv_style_set_text_color(&count_style, lv_color_white());
    lv_style_set_text_font(&count_style, &lv_font_montserrat_24);

    step_count_lbl = lv_label_create(scr);
    lv_obj_add_style(step_count_lbl, &count_style, 0);
    lv_label_set_text(step_count_lbl, "0");
    lv_obj_align(step_count_lbl, LV_ALIGN_CENTER, 0, -10);

    // "steps" sub-label
    static lv_style_t sub_style;
    lv_style_init(&sub_style);
    lv_style_set_text_color(&sub_style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_text_font(&sub_style, &lv_font_montserrat_16);

    lv_obj_t *steps_lbl = lv_label_create(scr);
    lv_obj_add_style(steps_lbl, &sub_style, 0);
    lv_label_set_text(steps_lbl, "steps today");
    lv_obj_align(steps_lbl, LV_ALIGN_CENTER, 0, 30);

    // Goal % label
    static lv_style_t goal_style;
    lv_style_init(&goal_style);
    lv_style_set_text_color(&goal_style, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_text_font(&goal_style, &lv_font_montserrat_16);

    step_goal_lbl = lv_label_create(scr);
    lv_obj_add_style(step_goal_lbl, &goal_style, 0);
    lv_label_set_text(step_goal_lbl, "0% of goal");
    lv_obj_align(step_goal_lbl, LV_ALIGN_BOTTOM_MID, 0, -50);

    activity_screen_update();
    return scr;
}
