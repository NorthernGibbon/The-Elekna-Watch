// start_screen.c
#include "start_screen.h"
#include <lvgl.h>

lv_obj_t * start_screen_create(void) {
    // Create a new screen object
    lv_obj_t * scr = lv_obj_create(NULL);

    // Create a top label on that screen
    lv_obj_t * top_lbl = lv_label_create(scr);
    lv_label_set_text(top_lbl, "-- The Elekna Watch --");
    lv_obj_align(top_lbl, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * middle_lbl = lv_label_create(scr);
    lv_label_set_text(middle_lbl, "-- The Start Screen --");
    lv_obj_align(middle_lbl, LV_ALIGN_TOP_MID, 0, 100); // Last number is vertical

    return scr;
}