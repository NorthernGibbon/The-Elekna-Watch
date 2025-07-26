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

#define TP_SDA_PIN  1
#define TP_SCL_PIN  3

// ─── SETUP & LOOP ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // Required by the I2C driver and TCA9554PWR
  pinMode(I2C_SDA_PIN, INPUT_PULLUP);
  pinMode(I2C_SCL_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000);
  
  delay(100);  // let USB-Serial wake up

  TCA9554PWR_Init();
  Backlight_Init();

  Touch_Init();
  LCD_Init();
  Lvgl_Init();
  ui_init();

  // — Init your RTC chip —
  PCF85063_Init();
}

void loop() {
    Touch_Loop();      
    Lvgl_Loop();       
    delay(5);
}



