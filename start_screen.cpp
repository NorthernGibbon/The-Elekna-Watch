#include "start_screen.h"
#include <lvgl.h>
#include <WiFi.h>
#include "wifi_manager.h"

static lv_obj_t *wifi_lbl = NULL;
static lv_obj_t *ip_lbl = NULL;

lv_obj_t * start_screen_create(void) {
    lv_obj_t * scr = lv_obj_create(NULL);

    // Main label
    lv_obj_t * top_lbl = lv_label_create(scr);
    lv_label_set_text(top_lbl, "-- The Elekna Watch --");
    lv_obj_align(top_lbl, LV_ALIGN_TOP_MID, 0, 50);

    // WiFi Status Label/Icon
    wifi_lbl = lv_label_create(scr);
    update_wifi_status_label(); // Set initial status
    lv_obj_align(wifi_lbl, LV_ALIGN_CENTER, 0, 20);

    ip_lbl = lv_label_create(scr);
    lv_label_set_text(ip_lbl, "IP: ...");
    lv_obj_align(ip_lbl, LV_ALIGN_CENTER, 0, 0);

    return scr;
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
}

void start_screen_enable_wifi_auto_update()
{
    lv_timer_create(wifi_status_timer_cb, 1000, NULL); 
}
