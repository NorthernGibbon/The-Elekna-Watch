#include <Arduino.h>
#include <lvgl.h>
#include "Display_ST77916.h"  // provides void LCDInit();
#include "Touch_CST816.h"
#include "TCA9554PWR.h"     // provides void Touch_Init();
#include "LVGL_Driver.h"
#include "lv_conf.h"          // your LVGL settings
#include "RTC_PCF85063.h" 
#include <Wire.h>
#include <sys/time.h>
#include "ui_manager.h"
#include "Wireless.h"
#include "wifi_manager.h"
#include "bluetooth_manager.h"

#define TP_SDA_PIN  1
#define TP_SCL_PIN  3

// WIFI config
const char* ssid = "NETGEAR44";
const char* password = "gentlefire780";

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

// ─── SETUP & LOOP ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

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

  wifi_manager_connect(ssid, password);
  wifi_manager_get_status();
  bluetooth_init();

  PCF85063_Init();
}

void loop() {
    Touch_Loop();      
    Lvgl_Loop();

    #if defined(ESP32)
    if (psramFound()) {
        Serial.println("PSRAM found!");
        Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    } else {
        Serial.println("PSRAM NOT FOUND!");
        while (1) delay(1000);
    }
  #endif
}