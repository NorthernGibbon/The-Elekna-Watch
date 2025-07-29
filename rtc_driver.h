// rtc_driver.h
#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include "RTC_PCF85063.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the PCF85063 RTC hardware.
 * Must be called once at startup.
 */
void RTC_Init(void);

/**
 * Set the date/time in the RTC. Provide local time.
 * @param dt Pointer to datetime_t struct with fields {year, month, day, dotw, hour, minute, second}.
 */
void RTC_SetDateTime(const datetime_t *dt);

/**
 * Read the current date/time from the RTC.
 * @param dt Pointer to datetime_t struct to fill in.
 * @return true on success.
 */
bool RTC_GetDateTime(datetime_t *dt);

/**
 * Synchronize RTC with NTP over Wi-Fi.
 * Requires Wi-Fi already connected.
 * @return true on success.
 */
bool RTC_SyncWithNTP();

#ifdef __cplusplus
}
#endif

#endif // RTC_DRIVER_H // RTC_DRIVER_H