#include "step_counter.h"
#include "Gyro_QMI8658.h"
#include "rtc_driver.h"
#include <Arduino.h>
#include <math.h>

#define STEP_THRESHOLD     1.15f   // G — magnitude peak to count as a step
#define STEP_DEBOUNCE_MS   250     // minimum ms between steps
#define POLL_INTERVAL_MS   20      // 50Hz polling

static volatile uint32_t step_count   = 0;
static uint8_t last_reset_day         = 0xFF;

static void check_daily_reset(void) {
    datetime_t now;
    RTC_GetDateTime(&now);
    if (now.day != last_reset_day) {
        step_count    = 0;
        last_reset_day = now.day;
    }
}

static void step_task(void *parameter) {
    float   filtered     = 1.0f;
    bool    above        = false;
    uint32_t last_step   = 0;

    while (1) {
        getAccelerometer();

        float mag = sqrtf(Accel.x * Accel.x +
                          Accel.y * Accel.y +
                          Accel.z * Accel.z);

        // Low-pass filter to smooth noise
        filtered = 0.8f * filtered + 0.2f * mag;

        if (!above && filtered > STEP_THRESHOLD) {
            above = true;
        } else if (above && filtered < STEP_THRESHOLD) {
            above = false;
            uint32_t now_ms = millis();
            if (now_ms - last_step > STEP_DEBOUNCE_MS) {
                step_count++;
                last_step = now_ms;
            }
        }

        check_daily_reset();
        vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
    }
}

void StepCounter_Init(void) {
    datetime_t now;
    RTC_GetDateTime(&now);
    last_reset_day = now.day;
    step_count = 0;
}

void StepCounter_StartTask(void) {
    xTaskCreatePinnedToCore(
        step_task,
        "StepTask",
        2048,
        NULL,
        1,
        NULL,
        1
    );
}

uint32_t StepCounter_GetSteps(void) {
    return step_count;
}
