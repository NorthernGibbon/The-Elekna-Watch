#include "start_screen.h"
#include <lvgl.h>
#include <WiFi.h>
#include "wifi_manager.h"
#include "bluetooth_manager.h"
#include "BAT_Driver.h"
#include "start_bg.h"
#include "rtc_driver.h"
#include "step_counter.h"

// lv object creation for labels
static lv_obj_t *wifi_lbl         = NULL;
static lv_obj_t *ip_lbl           = NULL;
static lv_obj_t *ble_lbl          = NULL;
static lv_obj_t *battery_lbl      = NULL;
static lv_obj_t *time_lbl         = NULL;
static lv_obj_t *seperator_lbl    = NULL;
static lv_obj_t *date_lbl         = NULL;
static lv_obj_t *top_lbl          = NULL;
static lv_obj_t *steps_lbl        = NULL;

static lv_style_t *style_ble;

lv_obj_t * start_screen_create(void) {
  lv_obj_t * scr = lv_obj_create(NULL);

  // Background image
  static lv_style_t bg_style;
  lv_style_init(&bg_style);
  lv_style_set_bg_image_src(&bg_style, &start_bg);
  lv_style_set_bg_image_opa(&bg_style, LV_OPA_COVER);
  lv_style_set_bg_opa(&bg_style, LV_OPA_COVER);
  lv_obj_add_style(scr, &bg_style, 0);

  // Style and init of top label
  static lv_style_t top_lbl_style;
  lv_style_init(&top_lbl_style);
  lv_style_set_text_color(&top_lbl_style, lv_color_white());
  lv_style_set_text_font(&top_lbl_style, &lv_font_montserrat_22);

  top_lbl = lv_label_create(scr);
  lv_obj_add_style(top_lbl, &top_lbl_style, LV_PART_MAIN);
  lv_label_set_text(top_lbl, "ELEKNA");
  lv_obj_align(top_lbl, LV_ALIGN_TOP_MID, 0, 40);

  // Style and init of digital clock label
  static lv_style_t time_style;
  lv_style_init(&time_style);
  lv_style_set_text_color(&time_style, lv_color_white());
  lv_style_set_text_font(&time_style, &lv_font_montserrat_20);

  time_lbl = lv_label_create(scr);
  lv_obj_add_style(time_lbl, &time_style, LV_PART_MAIN);
  lv_label_set_text(time_lbl, "00:00:00");
  lv_obj_align(time_lbl, LV_ALIGN_LEFT_MID, 50, -80);

  // Style and init for date label
  static lv_style_t date_style;
  lv_style_init(&date_style);
  lv_style_set_text_color(&date_style, lv_color_white());
  lv_style_set_text_font(&date_style, &lv_font_montserrat_16);

  date_lbl = lv_label_create(scr);
  lv_obj_add_style(date_lbl, &date_style, LV_PART_MAIN);
  lv_label_set_text(date_lbl, "0000-00-00");
  lv_obj_align(date_lbl, LV_ALIGN_LEFT_MID, 50, -60);

  // Style and init of wifi label
  static lv_style_t wifi_style;
  lv_style_init(&wifi_style);
  lv_style_set_text_color(&wifi_style, lv_color_white());
  lv_style_set_text_font(&wifi_style, &lv_font_montserrat_16);

  wifi_lbl = lv_label_create(scr);
  update_wifi_status_label(); 
  lv_obj_add_style(wifi_lbl, &wifi_style, LV_PART_MAIN);
  lv_obj_align(wifi_lbl, LV_ALIGN_LEFT_MID, 50, -30);

  // Style and init of battery label
  static lv_style_t bat_style;
  lv_style_init(&bat_style);
  lv_style_set_text_color(&bat_style, lv_color_white());
  lv_style_set_text_font(&bat_style, &lv_font_montserrat_16);

  battery_lbl = lv_label_create(scr);
  lv_obj_add_style(battery_lbl, &bat_style, LV_PART_MAIN);
  lv_obj_align(battery_lbl, LV_ALIGN_LEFT_MID, 50, -5);

  // Style and init of steps label
  static lv_style_t steps_style;
  lv_style_init(&steps_style);
  lv_style_set_text_color(&steps_style, lv_color_white());
  lv_style_set_text_font(&steps_style, &lv_font_montserrat_16);

  steps_lbl = lv_label_create(scr);
  lv_obj_add_style(steps_lbl, &steps_style, LV_PART_MAIN);
  lv_label_set_text(steps_lbl, LV_SYMBOL_CHARGE " 0 steps");
  lv_obj_align(steps_lbl, LV_ALIGN_LEFT_MID, 50, 20);

  // Style and init of ble label
  /*
  static lv_style_t ble_style;
  lv_style_init(&ble_style);
  lv_style_set_text_color(&ble_style, lv_color_white());
  lv_style_set_text_font(&ble_style, LV_FONT_DEFAULT);

  ble_lbl = lv_label_create(scr);
  lv_obj_add_style(ble_lbl, &ble_style, LV_PART_MAIN);
  lv_label_set_text(ble_lbl, LV_SYMBOL_BLUETOOTH " Not Connected");
  lv_obj_align(ble_lbl, LV_ALIGN_CENTER, 0, 40);
  */

  return scr;
}

static void update_battery_label(void) {
    if(!battery_lbl) return;
    float pct = BAT_Get_Percent();

    const char* bat_sym;
    if      (pct > 80) bat_sym = LV_SYMBOL_BATTERY_FULL;
    else if (pct > 60) bat_sym = LV_SYMBOL_BATTERY_3;
    else if (pct > 40) bat_sym = LV_SYMBOL_BATTERY_2;
    else if (pct > 20) bat_sym = LV_SYMBOL_BATTERY_1;
    else               bat_sym = LV_SYMBOL_BATTERY_EMPTY;

    lv_label_set_text_fmt(battery_lbl, "%s // %3d%%", bat_sym, int(pct));
}

void update_bluetooth_status_label() {
    if (!ble_lbl) return;
    if (bluetooth_is_connected()) {
        lv_label_set_text_fmt(ble_lbl, "%s Connected", LV_SYMBOL_BLUETOOTH);
    } else {
        lv_label_set_text_fmt(ble_lbl, "%s Not Connected", LV_SYMBOL_BLUETOOTH);
    }
}

static void update_time_label(void) {
    if(!time_lbl) return;
    datetime_t now;
    RTC_GetDateTime(&now);
    lv_label_set_text_fmt(
      time_lbl,
      "%02u:%02u:%02u",
      now.hour,
      now.minute,
      now.second
    );
}

static void update_date_label(void) {
    if(!date_lbl) return;
    datetime_t now;
    RTC_GetDateTime(&now);
    lv_label_set_text_fmt(
      date_lbl,
      "%04u / %02u / %02u",
      now.year,
      now.month,
      now.day
    );
}

void update_wifi_status_label() {
    if (!wifi_lbl) return;

    WifiStatus st = wifi_manager_get_status();
    switch (st) {
        case WIFI_CONNECTED: {
          String ip = wifi_manager_get_ip();
          lv_label_set_text_fmt(wifi_lbl, "%s // %s", LV_SYMBOL_WIFI, ip.c_str());
          break;
        }   
        case WIFI_CONNECTING:
            lv_label_set_text_fmt(wifi_lbl, "%s Connecting...", LV_SYMBOL_WIFI);
            break;
        case WIFI_FAILED:
            lv_label_set_text_fmt(wifi_lbl, "%s Failed", LV_SYMBOL_WIFI);
            break;
        case WIFI_DISCONNECTED:
        default:
            lv_label_set_text_fmt(wifi_lbl, "%s // Not Connected", LV_SYMBOL_WIFI);
            break;
  }
}

static void update_steps_label(void) {
    if (!steps_lbl) return;
    lv_label_set_text_fmt(steps_lbl, LV_SYMBOL_CHARGE " %lu steps", StepCounter_GetSteps());
}

static void wifi_status_timer_cb(lv_timer_t * timer)
{
  static int tick = 0;
  update_wifi_status_label();
  update_bluetooth_status_label();
  update_steps_label();
  // Calls the update battery every 30 seconds instead to remove jitter in the ui
  if (++tick >= 30) {
    update_battery_label();
    tick = 0;
  }
  update_time_label();
  update_date_label();
}

void start_screen_enable_battery_auto_update()
{
    lv_timer_create([](lv_timer_t*){ update_battery_label(); }, 30000, NULL);
}

void start_screen_enable_wifi_auto_update()
{
    lv_timer_create(wifi_status_timer_cb, 1000, NULL); 
}

void start_screen_enable_ble_auto_update()
{
    lv_timer_create([](lv_timer_t*){ update_bluetooth_status_label(); }, 1000, NULL);
}