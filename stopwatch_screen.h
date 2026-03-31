#pragma once
#include <lvgl.h>

lv_obj_t * stopwatch_screen_create(void);
void       stopwatch_handle_tap(int raw_x, int raw_y);
