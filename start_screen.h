#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <lvgl.h>

lv_obj_t * start_screen_create(void);

void update_wifi_status_label(void);
void update_bluetooth_status_label(void);

void start_screen_enable_wifi_auto_update(void);
void start_screen_enable_battery_auto_update(void);
void start_screen_enable_ble_auto_update(void);

#endif // START_SCREEN_H
