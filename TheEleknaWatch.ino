#include <Arduino.h>
#include <lvgl.h>
#include "Display_ST77916.h"
#include "Touch_CST816.h"
#include "TCA9554PWR.h"
#include "LVGL_Driver.h"
#include "lv_conf.h"
#include "RTC_PCF85063.h"
#include <Wire.h>
#include <sys/time.h>
#include <time.h>
#include "ui_manager.h"
#include "Wireless.h"
#include "wifi_manager.h"
#include "bluetooth_manager.h"
#include "BAT_Driver.h"
#include "PWR_Key.h"
#include "clock_screen.h"
#include "Gyro_QMI8658.h"
#include "step_counter.h"
#include "activity_screen.h"

#define TP_SDA_PIN  1
#define TP_SCL_PIN  3

#define RTC_INT_PIN GPIO_NUM_9
static volatile bool rtc_tick = false;

static bool ntp_synced = false;

struct WifiCredential {
  const char* ssid;
  const char* password;
};

WifiCredential wifiCandidates[] = {
  {"NETGEAR44", "gentlefire780"},
  {"Elekna", "gibbon11"}
};

constexpr int WIFI_CANDIDATE_COUNT = sizeof(wifiCandidates) / sizeof(wifiCandidates[0]);

void wifi_connect_best() {
  for (int i = 0; i < WIFI_CANDIDATE_COUNT; ++i) {
    Serial.print("Trying SSID: ");
    Serial.println(wifiCandidates[i].ssid);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(200);

    wifi_manager_connect(wifiCandidates[i].ssid, wifiCandidates[i].password);

    uint32_t start = millis();
    while (millis() - start < 12000) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected to: ");
        Serial.println(wifiCandidates[i].ssid);
        return;
      }
      delay(200);
    }

    Serial.print("Failed to connect to: ");
    Serial.println(wifiCandidates[i].ssid);
  }

  Serial.println("Failed to connect to any known WiFi");
}


static void IRAM_ATTR rtc_isr_handler(void* arg) {
  rtc_tick = true;
}

static void setup_rtc_interrupt() {
  gpio_install_isr_service(0);

  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = 1ULL << RTC_INT_PIN;
  io_conf.mode         = GPIO_MODE_INPUT;
  io_conf.pull_up_en   = GPIO_PULLUP_ENABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.intr_type    = GPIO_INTR_POSEDGE;
  gpio_config(&io_conf);

  gpio_isr_handler_add(RTC_INT_PIN, rtc_isr_handler, nullptr);
}

void DriverTask(void *parameter) {
  while(1) {
    PWR_Loop();
    BAT_Get_Volts();
    BAT_Get_Percent();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void Driver_Loop() {
  xTaskCreatePinnedToCore(
    DriverTask,
    "DriverTask",
    4096,
    NULL,
    3,
    NULL,
    0
  );
}

void seedRTCwithNTP() {
  configTzTime(
    "CET-1CEST-2,"
    "M3.5.0/02:00:00,"
    "M10.5.0/03:00:00",
    "pool.ntp.org",
    "time.nist.gov"
  );

  struct tm timeinfo;
  Serial.print("Waiting for NTP… ");
  uint32_t start = millis();
  while (millis() - start < 10000) {
    if (getLocalTime(&timeinfo)) break;
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get time from NTP");
    return;
  }

  datetime_t now = {
    .year   = (uint16_t)(timeinfo.tm_year + 1900),
    .month  = (uint8_t)(timeinfo.tm_mon + 1),
    .day    = (uint8_t)timeinfo.tm_mday,
    .dotw   = (uint8_t)timeinfo.tm_wday,
    .hour   = (uint8_t)timeinfo.tm_hour,
    .minute = (uint8_t)timeinfo.tm_min,
    .second = (uint8_t)timeinfo.tm_sec
  };

  PCF85063_Set_All(now);
  Serial.printf(
    "RTC seeded -> %04u-%02u-%02u %02u:%02u:%02u (dotw %u)\n",
    now.year, now.month, now.day,
    now.hour, now.minute, now.second,
    now.dotw
  );
}

void setup() {
  Serial.begin(115200);

  PWR_Init();
  BAT_Init();

  pinMode(I2C_SDA_PIN, INPUT_PULLUP);
  pinMode(I2C_SCL_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);

  QMI8658_Init();

  TCA9554PWR_Init(0x00);
  Backlight_Init();

  Touch_Init();
  LCD_Init();
  Lvgl_Init();
  ui_init();
  Driver_Loop();

  wifi_connect_best();
  wifi_manager_get_status();

  if (WiFi.status() == WL_CONNECTED) {
    seedRTCwithNTP();
    ntp_synced = true;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi off — NTP sync done");
  }

  PCF85063_Init();
  PCF85063_Enable_1Hz_Timer();
  setup_rtc_interrupt();

  StepCounter_Init();
  StepCounter_StartTask();
}

void loop() {
  if (rtc_tick) {
    rtc_tick = false;
    datetime_t now;
    PCF85063_Read_Time(&now);
    clock_screen_update();
    activity_screen_update();
  }

  Touch_Loop();
  Lvgl_Loop();

  delay(10);
}
