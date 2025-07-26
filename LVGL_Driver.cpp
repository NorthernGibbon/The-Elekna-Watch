#include "LVGL_Driver.h"
#include "RTC_PCF85063.h"    // your RTC driver
#include <string.h>          // for memset
#include "esp_heap_caps.h" // Required for heap_caps_malloc
#include <Arduino.h>

#define HOR_RES            360
#define VER_RES            360
#define LVGL_BUF_PIXELS    (HOR_RES * VER_RES)
#define LVGL_BUF_SIZE      (LVGL_BUF_PIXELS * sizeof(lv_color_t))


static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

/* Display flushing: send px_map to your LCD */
void Lvgl_Display_LCD(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * buf16 = (uint16_t *)px_map; 
    LCD_addWindow(area->x1, area->y1, area->x2, area->y2, buf16);
    lv_display_flush_ready(disp);
}

/* Touchpad read callback (unchanged) */
void Lvgl_Touchpad_Read(lv_indev_t * indev, lv_indev_data_t * data)
{
    Touch_Read_Data();
    if(touch_data.points) {
        data->point.x = touch_data.x;
        data->point.y = touch_data.y;
        data->state   = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    memset(&touch_data, 0, sizeof(touch_data));
}

/* Called by your periodic timer: */
void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void Lvgl_Init(void)
{
    lv_init();

    /*–– Display setup ––*/
    lv_display_t * disp = lv_display_create(HOR_RES, VER_RES);
    lv_display_set_flush_cb(disp, Lvgl_Display_LCD);

    // --- Full screen double buffer in PSRAM ---
    buf1 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE, MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE, MALLOC_CAP_SPIRAM);
    if (!buf1 || !buf2) {
        Serial.println("Full buffer allocation failed! Using small fallback buffers.");
        static lv_color_t fallback_buf1[HOR_RES * 10];
        static lv_color_t fallback_buf2[HOR_RES * 10];
        buf1 = fallback_buf1;
        buf2 = fallback_buf2;
        lv_display_set_buffers(disp, buf1, buf2, sizeof(fallback_buf1), LV_DISPLAY_RENDER_MODE_FULL);
    } else {
        lv_display_set_buffers(disp, buf1, buf2, LVGL_BUF_SIZE, LV_DISPLAY_RENDER_MODE_FULL);
    }

    /*–– Touch input ––*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, Lvgl_Touchpad_Read);
}


void Lvgl_Loop(void)
{
    lv_tick_inc(5);   
    lv_timer_handler();
}
