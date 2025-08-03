#include "start_screen.h"
#include <lvgl.h>
#include <WiFi.h>
#include "wifi_manager.h"
#include "bluetooth_manager.h"

static lv_obj_t *wifi_lbl = NULL;
static lv_obj_t *ip_lbl = NULL;
static lv_obj_t *ble_lbl = NULL;

static lv_style_t *style_wifi;
static lv_style_t *style_ble;

lv_obj_t * start_screen_create(void) {
    lv_obj_t * scr = lv_obj_create(NULL);

    // Background color and opacity (Maybe i need to create a global style for each screen but this works)
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    // Style and init of top label
    static lv_style_t top_lbl_style;
    lv_style_init(&top_lbl_style);
    lv_style_set_text_color(&top_lbl_style, lv_color_white());
    lv_style_set_text_font(&top_lbl_style, LV_FONT_DEFAULT);

    lv_obj_t * top_lbl = lv_label_create(scr);
    lv_obj_add_style(top_lbl, &top_lbl_style, LV_PART_MAIN);
    lv_label_set_text(top_lbl, "-- The Elekna Watch --");
    lv_obj_align(top_lbl, LV_ALIGN_TOP_MID, 0, 50);

    // Style and init of ip label
    static lv_style_t ip_style;
    lv_style_init(&ip_style);
    lv_style_set_text_color(&ip_style, lv_color_white());
    lv_style_set_text_font(&ip_style, LV_FONT_DEFAULT);

    ip_lbl = lv_label_create(scr);
    lv_obj_add_style(ip_lbl, &ip_style, LV_PART_MAIN);
    lv_label_set_text(ip_lbl, "IP: ...");
    lv_obj_align(ip_lbl, LV_ALIGN_CENTER, 0, 0);

    // Style and init of wifi label
    static lv_style_t wifi_style;
    lv_style_init(&wifi_style);
    lv_style_set_text_color(&wifi_style, lv_color_white());
    lv_style_set_text_font(&wifi_style, LV_FONT_DEFAULT);

    wifi_lbl = lv_label_create(scr);
    update_wifi_status_label(); 
    lv_obj_add_style(wifi_lbl, &wifi_style, LV_PART_MAIN);
    lv_obj_align(wifi_lbl, LV_ALIGN_CENTER, 0, 20);

    // Style and init of ble label
    static lv_style_t ble_style;
    lv_style_init(&ble_style);
    lv_style_set_text_color(&ble_style, lv_color_white());
    lv_style_set_text_font(&ble_style, LV_FONT_DEFAULT);

    ble_lbl = lv_label_create(scr);
    lv_obj_add_style(ble_lbl, &ble_style, LV_PART_MAIN);
    lv_label_set_text(ble_lbl, LV_SYMBOL_BLUETOOTH " Not Connected");
    lv_obj_align(ble_lbl, LV_ALIGN_CENTER, 0, 40);

    return scr;
}

void update_bluetooth_status_label() {
    if (!ble_lbl) return;
    if (bluetooth_is_connected()) {
        lv_label_set_text_fmt(ble_lbl, "%s Connected", LV_SYMBOL_BLUETOOTH);
    } else {
        lv_label_set_text_fmt(ble_lbl, "%s Not Connected", LV_SYMBOL_BLUETOOTH);
    }
}

void update_wifi_status_label() {
    if (!wifi_lbl) return;

    WifiStatus st = wifi_manager_get_status();
    switch (st) {
        case WIFI_CONNECTED:
            lv_label_set_text_fmt(wifi_lbl, "%s Connected", LV_SYMBOL_WIFI);
            if (ip_lbl)
                lv_label_set_text_fmt(ip_lbl, "IP: %s", wifi_manager_get_ip().c_str());
            break;
        case WIFI_CONNECTING:
            lv_label_set_text_fmt(wifi_lbl, "%s Connecting...", LV_SYMBOL_WIFI);
            if (ip_lbl)
                lv_label_set_text(ip_lbl, "IP: ...");
            break;
        case WIFI_FAILED:
            lv_label_set_text_fmt(wifi_lbl, "%s Failed", LV_SYMBOL_WIFI);
            if (ip_lbl)
                lv_label_set_text(ip_lbl, "IP: ...");
            break;
        case WIFI_DISCONNECTED:
        default:
            lv_label_set_text_fmt(wifi_lbl, "%s Not Connected", LV_SYMBOL_WIFI);
            if (ip_lbl)
                lv_label_set_text(ip_lbl, "IP: ...");
            break;
    }
}

static void wifi_status_timer_cb(lv_timer_t * timer)
{
    update_wifi_status_label();
    update_bluetooth_status_label();
}

void start_screen_enable_wifi_auto_update()
{
    lv_timer_create(wifi_status_timer_cb, 1000, NULL); 
}

void start_screen_enable_ble_auto_update()
{
    lv_timer_create([](lv_timer_t*){ update_bluetooth_status_label(); }, 1000, NULL);
}