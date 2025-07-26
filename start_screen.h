#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <lvgl.h>

lv_obj_t * start_screen_create(void);

// Update the WiFi status label (call if you want to force refresh)
void update_wifi_status_label(void);

// Enable automatic updating of the WiFi label with a timer
void start_screen_enable_wifi_auto_update(void);

#endif // START_SCREEN_H
