#include "LVGL_Driver.h"
#include "RTC_PCF85063.h"
#include <string.h>
#include <Arduino.h>
#include "esp_heap_caps.h"

#define HOR_RES            360
#define VER_RES            360
#define LVGL_BUF_PIXELS    (HOR_RES * 20)
#define LVGL_BUF_SIZE      (LVGL_BUF_PIXELS * sizeof(lv_color_t))

static lv_color_t *buf1    = NULL;
static lv_color_t *buf2    = NULL;
static lv_color_t *rot_buf = NULL;

void Lvgl_Display_LCD(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    uint16_t *src = (uint16_t *)px_map;
    uint16_t *dst = (uint16_t *)rot_buf;

    // 180° rotation: (x, y) -> (w-1-x, h-1-y)
    for (int32_t sy = 0; sy < h; sy++) {
        for (int32_t sx = 0; sx < w; sx++) {
            int32_t si = sy * w + sx;

            int32_t dx = w - 1 - sx;
            int32_t dy = h - 1 - sy;
            int32_t di = dy * w + dx;

            dst[di] = src[si];
        }
    }

    // Area mapping for 180°
    int32_t x1r = HOR_RES - 1 - area->x2;
    int32_t x2r = HOR_RES - 1 - area->x1;
    int32_t y1r = VER_RES - 1 - area->y2;
    int32_t y2r = VER_RES - 1 - area->y1;

    LCD_addWindow(x1r, y1r, x2r, y2r, dst);
    lv_display_flush_ready(disp);
}


void Lvgl_Touchpad_Read(lv_indev_t * indev, lv_indev_data_t * data)
{
    // Use touch_data populated by Touch_Loop via the ISR — do NOT re-read
    // from hardware here, as the CST816 clears its registers after one read.
    if (touch_data.points) {
        data->point.x = HOR_RES - 1 - touch_data.x;
        data->point.y = VER_RES - 1 - touch_data.y;
        data->state   = LV_INDEV_STATE_PRESSED;
        touch_data.points = 0;  // consume: next call reports RELEASED
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}


void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void print_psram_stats(const char *tag) {
    size_t total   = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    Serial.printf("%s → PSRAM free: %u  largest block: %u\n",
                  tag, (uint32_t)total, (uint32_t)largest);
}

void Lvgl_Init(void)
{
    lv_init();

    lv_display_t * disp = lv_display_create(HOR_RES, VER_RES);
    lv_display_set_flush_cb(disp, Lvgl_Display_LCD);

    Serial.printf("Before buffer: PSRAM free: %d Heap free: %d\n",
                  ESP.getFreePsram(), ESP.getFreeHeap());

    buf1 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE,
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    buf2 = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE,
                                          MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    rot_buf = (lv_color_t *)heap_caps_malloc(LVGL_BUF_SIZE,
                                             MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!buf1 || !buf2 || !rot_buf) {
        Serial.println("PSRAM allocation failed! System will halt.");
        while (1) {
            delay(1000);
        }
    }

    Serial.printf("After buffer: PSRAM free: %d Heap free: %d\n",
                  ESP.getFreePsram(), ESP.getFreeHeap());

    lv_display_set_buffers(disp, buf1, buf2, LVGL_BUF_SIZE,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, Lvgl_Touchpad_Read);
    lv_timer_set_period(lv_indev_get_read_timer(indev), 5);  // read every ~10ms real time
}

void Lvgl_Loop(void)
{
    lv_tick_inc(5);
    lv_timer_handler();
}
