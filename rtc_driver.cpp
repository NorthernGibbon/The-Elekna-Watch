// rtc_driver.cpp
#include "rtc_driver.h"
#include "RTC_PCF85063.h"
#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include <lwip/apps/sntp.h>

void RTC_Init(void) {
    PCF85063_Init();
}

void RTC_SetDateTime(const datetime_t *dt) {
    PCF85063_Set_All(*dt);
}

bool RTC_GetDateTime(datetime_t *dt) {
    PCF85063_Read_Time(dt);
    return true;
}

bool RTC_SyncWithNTP() {
    configTzTime("CET-1CEST-2,M3.5.0/2,M10.5.0/3",
                 "pool.ntp.org", "time.nist.gov");

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo, 10000)) {
        Serial.println("RTC_SyncWithNTP: SNTP time fetch failed");
        return false;
    }

    datetime_t dt;
    dt.year   = timeinfo.tm_year + 1900;
    dt.month  = timeinfo.tm_mon + 1;
    dt.day    = timeinfo.tm_mday;
    dt.dotw   = timeinfo.tm_wday;  
    dt.hour   = timeinfo.tm_hour;
    dt.minute = timeinfo.tm_min;
    dt.second = timeinfo.tm_sec;

    RTC_SetDateTime(&dt);

    Serial.printf(
      "RTC_SyncWithNTP: Set RTC to %04u-%02u-%02u %02u:%02u:%02u\n",
      dt.year, dt.month, dt.day,
      dt.hour, dt.minute, dt.second
    );
    return true;
}
