#include "PWR_Key.h"
#include "Touch_CST816.h"
#include "esp_sleep.h"

static uint8_t BAT_State = 0; 
static uint8_t Device_State = 0; 
static uint16_t Long_Press = 0;

static uint32_t lastActivityMs = 0;
static bool isSleeping = false;
static uint8_t savedBacklight = 50;

static const uint32_t AUTO_SLEEP_TIMEOUT_MS = 1UL * 600UL * 1000UL;

void PWR_NotifyActivity(void)
{
  lastActivityMs = millis();
}

bool PWR_IsSleeping(void)
{
  return isSleeping;
}

void Fall_Asleep(void)
{
  if (isSleeping) return;
  isSleeping = true;
  savedBacklight = LCD_Backlight;
  LCD_Backlight = 0;
  Set_Backlight(LCD_Backlight);

  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)CST816_INT_PIN, 0);

  esp_light_sleep_start();

  isSleeping = false;
  if (savedBacklight == 0) savedBacklight = 50;
  LCD_Backlight = savedBacklight;
  Set_Backlight(LCD_Backlight);
  lastActivityMs = millis();
}

void Restart(void)
{
  esp_restart();
}

void Shutdown(void)
{
  digitalWrite(PWR_Control_PIN, LOW);
  LCD_Backlight = 0;           
}

void PWR_Init(void)
{
  pinMode(PWR_KEY_Input_PIN, INPUT);    
  pinMode(PWR_Control_PIN, OUTPUT);
  digitalWrite(PWR_Control_PIN, LOW);
  vTaskDelay(100);
  if (!digitalRead(PWR_KEY_Input_PIN)) {   
    BAT_State = 1;         
    lastActivityMs = millis();      
    digitalWrite(PWR_Control_PIN, HIGH);
  }
}

void PWR_Loop(void)
{
  if (BAT_State) { 
    if (!digitalRead(PWR_KEY_Input_PIN)) {   
      if (BAT_State == 2) {         
        Long_Press++;
        if (Long_Press >= Device_Sleep_Time) {
          if (Long_Press >= Device_Sleep_Time && Long_Press < Device_Restart_Time)
            Device_State = 1;
          else if (Long_Press >= Device_Restart_Time && Long_Press < Device_Shutdown_Time)
            Device_State = 2;
          else if (Long_Press >= Device_Shutdown_Time)
            Shutdown(); 
        }
      }
    } else {
      if (BAT_State == 1)   
        BAT_State = 2;
      Long_Press = 0;
    }
  }

  // DEV MODE: sleep disabled to allow flashing
  // if (Device_State == 1) {
  //   Device_State = 0;
  //   Fall_Asleep();
  // } else if (Device_State == 2) {
  if (Device_State == 2) {
    Device_State = 0;
    Restart();
  }

  // uint32_t now = millis();
  // if (lastActivityMs == 0) lastActivityMs = now;
  // if (now - lastActivityMs >= AUTO_SLEEP_TIMEOUT_MS) {
  //   Fall_Asleep();
  // }
}
