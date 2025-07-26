#include "clock_screen.h"
#include <lvgl.h>

lv_obj_t * clock_screen_create(void) {
    // Create a new screen object
    lv_obj_t * scr = lv_obj_create(NULL);

    // Create a top label on that screen
    lv_obj_t * clock_top_lbl = lv_label_create(scr);
    lv_label_set_text(clock_top_lbl, "-- The Elekna Watch --");

    // Position it at the top center, with a little vertical offset
    lv_obj_align(clock_top_lbl, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * clock_middle_lbl = lv_label_create(scr);
    lv_label_set_text(clock_middle_lbl, "The Clock Screen");
    lv_obj_align(clock_middle_lbl, LV_ALIGN_TOP_MID, 0, 100);

    return scr;
}