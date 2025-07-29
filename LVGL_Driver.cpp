#include "LVGL_Driver.h"
#include "RTC_PCF85063.h"    // your RTC driver
#include <string.h>          // for memset
#include <Arduino.h>
#include "esp_heap_caps.h"

#define HOR_RES            360
#define VER_RES            360
#define LVGL_BUF_PIXELS    (HOR_RES * 20)
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

void print_psram_stats(const char *tag) {
    size_t total   = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    Serial.printf("%s → PSRAM free: %u  largest block: %u\n",
                  tag, uint32_t(total), uint32_t(largest));
}

void Lvgl_Init(void)
{
    lv_init();

    /*–– Display setup ––*/
    lv_display_t * disp = lv_display_create(HOR_RES, VER_RES);
    lv_display_set_flush_cb(disp, Lvgl_Display_LCD);

    Serial.printf("Before buffer: PSRAM free: %d Heap free: %d\n", ESP.getFreePsram(), ESP.getFreeHeap());

    buf1 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE,
                                          MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    buf2 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE,
                                          MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);

    Serial.printf("After buffer: PSRAM free: %d Heap free: %d\n", ESP.getFreePsram(), ESP.getFreeHeap());
    if (!buf1 || !buf2) {
    Serial.println("PSRAM allocation failed! System will halt.");
    while (1) delay(1000);
    } else {
      lv_display_set_buffers(disp, buf1, buf2, LVGL_BUF_SIZE,
      LV_DISPLAY_RENDER_MODE_PARTIAL);
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
