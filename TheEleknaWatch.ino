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

#define TP_SDA_PIN  1
#define TP_SCL_PIN  3

#define RTC_INT_PIN GPIO_NUM_9   
static volatile bool rtc_tick = false;

// WIFI config
const char* ssid = "NETGEAR44";
const char* password = "gentlefire780";

// RTC
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

void scan_wifi() {
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) Serial.println("No networks found");
  else {
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(WiFi.SSID(i));
    }
  }
}

void DriverTask(void *parameter) {
  while(1){
    PWR_Loop();
    BAT_Get_Volts();
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
  // 1) Configure Stockholm TZ (CET/CEST)
  configTzTime(
    "CET-1CEST-2,"
    "M3.5.0/02:00:00,"
    "M10.5.0/03:00:00",
    "pool.ntp.org",
    "time.nist.gov"
  );

  // 2) Wait up to 10 s for NTP
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
    .year   = timeinfo.tm_year + 1900,
    .month  = timeinfo.tm_mon  + 1,
    .day    = timeinfo.tm_mday,
    .dotw   = timeinfo.tm_wday,
    .hour   = timeinfo.tm_hour,
    .minute = timeinfo.tm_min,
    .second = timeinfo.tm_sec
  };

  PCF85063_Set_All(now);
  Serial.printf(
    "RTC seeded → %04u-%02u-%02u %02u:%02u:%02u (dotw %u)\n",
    now.year, now.month, now.day,
    now.hour, now.minute, now.second,
    now.dotw
  );
}


void setup() {
  Serial.begin(115200);
  PWR_Init();
  BAT_Init();

  scan_wifi();

  pinMode(I2C_SDA_PIN, INPUT_PULLUP);
  pinMode(I2C_SCL_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);
  
  delay(100); 

  TCA9554PWR_Init();
  Backlight_Init();

  Touch_Init();
  LCD_Init();
  Lvgl_Init();
  ui_init();
  Driver_Loop();

  wifi_manager_connect(ssid, password);
  wifi_manager_get_status();
  bluetooth_init();

  PCF85063_Init();
  seedRTCwithNTP(); 
  PCF85063_Enable_1Hz_Timer();
  setup_rtc_interrupt();
}

void loop() {

    if (rtc_tick) {
      rtc_tick = false;
      datetime_t now;
      PCF85063_Read_Time(&now);
      Serial.printf("RTC read: %02u:%02u:%02u\n", now.hour, now.minute, now.second);
      clock_screen_update();
    }

    Touch_Loop();      
    Lvgl_Loop();
}