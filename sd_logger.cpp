#include "sd_logger.h"
#include "rtc_driver.h"
#include "TCA9554PWR.h"
#include <SD_MMC.h>
#include <Arduino.h>

#define SD_CLK_PIN   14
#define SD_CMD_PIN   17
#define SD_D0_PIN    16

#define STEPS_TODAY_PATH   "/steps_today.txt"
#define STEPS_HISTORY_PATH "/steps_history.csv"

static bool sd_ready = false;

static void get_date_str(char *buf, size_t len) {
    datetime_t now;
    RTC_GetDateTime(&now);
    snprintf(buf, len, "%04u-%02u-%02u", now.year, now.month, now.day);
}

// If steps_today.txt exists and is from a previous day, append it to history.
static void archive_if_old(void) {
    File f = SD_MMC.open(STEPS_TODAY_PATH, FILE_READ);
    if (!f) return;

    char buf[32];
    size_t n = f.readBytes(buf, sizeof(buf) - 1);
    f.close();
    buf[n] = '\0';

    char today[12];
    get_date_str(today, sizeof(today));

    if (strncmp(buf, today, 10) != 0) {
        // Strip trailing newline for clean CSV
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';

        File hist = SD_MMC.open(STEPS_HISTORY_PATH, FILE_APPEND);
        if (hist) {
            hist.println(buf);
            hist.close();
            Serial.printf("SD: archived old entry -> %s\n", buf);
        }
        // Remove today file so it gets recreated fresh
        SD_MMC.remove(STEPS_TODAY_PATH);
    }
}

bool SD_Logger_Init(void) {
    // D3 must be HIGH for the SD card to operate in SDIO mode
    Set_EXIO(EXIO_PIN3, High);
    delay(20);

    SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);

    if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode
        Serial.println("SD: mount failed");
        sd_ready = false;
        return false;
    }

    Serial.println("SD: mounted OK");
    sd_ready = true;
    return true;
}

void SD_Logger_LoadSteps(uint32_t *out_steps) {
    *out_steps = 0;
    if (!sd_ready) return;

    archive_if_old();

    File f = SD_MMC.open(STEPS_TODAY_PATH, FILE_READ);
    if (!f) return;

    char buf[32];
    size_t n = f.readBytes(buf, sizeof(buf) - 1);
    f.close();
    buf[n] = '\0';

    char today[12];
    get_date_str(today, sizeof(today));

    if (strncmp(buf, today, 10) == 0) {
        char *comma = strchr(buf, ',');
        if (comma) {
            *out_steps = (uint32_t)atol(comma + 1);
            Serial.printf("SD: loaded %lu steps for %s\n", *out_steps, today);
        }
    }
}

void SD_Logger_SaveSteps(uint32_t steps) {
    if (!sd_ready) return;

    // Handle midnight rollover: archive old entry if date changed
    archive_if_old();

    char today[12];
    get_date_str(today, sizeof(today));

    File f = SD_MMC.open(STEPS_TODAY_PATH, FILE_WRITE);
    if (!f) {
        Serial.println("SD: failed to open steps_today.txt for write");
        return;
    }
    f.printf("%s,%lu\n", today, steps);
    f.close();
}

int SD_Logger_GetHistory(uint32_t *out, int max_count) {
    if (!sd_ready || max_count <= 0) return 0;

    File f = SD_MMC.open(STEPS_HISTORY_PATH, FILE_READ);
    if (!f) return 0;

    // Read all lines into a temporary rolling buffer
    #define MAX_HIST_LINES 64
    uint32_t vals[MAX_HIST_LINES];
    int      total = 0;

    char line[32];
    int  li = 0;
    while (f.available()) {
        char c = f.read();
        if (c == '\n' || c == '\r') {
            if (li > 0) {
                line[li] = '\0';
                char *comma = strchr(line, ',');
                if (comma) {
                    uint32_t v = (uint32_t)atol(comma + 1);
                    if (total < MAX_HIST_LINES) vals[total++] = v;
                }
                li = 0;
            }
        } else if (li < (int)sizeof(line) - 1) {
            line[li++] = c;
        }
    }
    f.close();

    // Return last max_count entries (oldest first)
    int start = total > max_count ? total - max_count : 0;
    int count = total - start;
    for (int i = 0; i < count; i++) out[i] = vals[start + i];
    return count;
}
