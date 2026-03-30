#include "start_screen.h"
#include <lvgl.h>
#include <WiFi.h>
#include "wifi_manager.h"
#include "bluetooth_manager.h"
#include "BAT_Driver.h"
#include "rtc_driver.h"
#include "step_counter.h"
#include "C_Converts/icons.h"

// ── Retro green palette ──────────────────────────────────────
#define CLR_GREEN_HI  lv_color_make(0, 255,  65)
#define CLR_GREEN_DIM lv_color_make(0, 160,  40)
#define CLR_GREEN_DBG lv_color_make(0,  60,  15)
#define CLR_BLACK     lv_color_black()

// ── Widget handles ───────────────────────────────────────────
static lv_obj_t *time_lbl  = NULL;
static lv_obj_t *date_lbl  = NULL;
static lv_obj_t *wifi_lbl  = NULL;
static lv_obj_t *wifi_img  = NULL;
static lv_obj_t *bat_lbl   = NULL;
static lv_obj_t *bat_img   = NULL;
static lv_obj_t *steps_lbl = NULL;

// ── Helpers ──────────────────────────────────────────────────
static void update_time_label(void) {
    if (!time_lbl) return;
    datetime_t now;
    RTC_GetDateTime(&now);
    lv_label_set_text_fmt(time_lbl, "%02u:%02u:%02u", now.hour, now.minute, now.second);
}

static void update_date_label(void) {
    if (!date_lbl) return;
    datetime_t now;
    RTC_GetDateTime(&now);
    static const char *days[]   = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
    static const char *months[] = {"JAN","FEB","MAR","APR","MAY","JUN",
                                   "JUL","AUG","SEP","OCT","NOV","DEC"};
    lv_label_set_text_fmt(date_lbl, "%s  %02u %s %04u",
        days[now.dotw], now.day, months[now.month - 1], now.year);
}

static void update_battery(void) {
    if (!bat_lbl || !bat_img) return;
    float pct = BAT_Get_Percent();
    lv_label_set_text_fmt(bat_lbl, "%3d%%", (int)pct);
    if      (pct > 75) lv_img_set_src(bat_img, &Battery_100_Icon);
    else if (pct > 50) lv_img_set_src(bat_img, &Battery_75_Icon);
    else if (pct > 25) lv_img_set_src(bat_img, &Battery_50_Icon);
    else               lv_img_set_src(bat_img, &Battery_25_Icon);
}

void update_wifi_status_label(void) {
    if (!wifi_lbl || !wifi_img) return;
    WifiStatus st = wifi_manager_get_status();
    switch (st) {
        case WIFI_CONNECTED: {
            String ip = wifi_manager_get_ip();
            lv_label_set_text_fmt(wifi_lbl, "%s", ip.c_str());
            break;
        }
        case WIFI_CONNECTING:
            lv_label_set_text(wifi_lbl, "CONNECTING..");
            break;
        default:
            lv_label_set_text(wifi_lbl, "NO SIGNAL");
            break;
    }
}

void update_bluetooth_status_label(void) { }

static void update_steps(void) {
    if (!steps_lbl) return;
    lv_label_set_text_fmt(steps_lbl, "STEPS  %05lu", StepCounter_GetSteps());
}

// ── Timer callback ───────────────────────────────────────────
static void ui_timer_cb(lv_timer_t *) {
    static int tick = 0;
    update_time_label();
    update_date_label();
    update_wifi_status_label();
    update_steps();
    if (++tick >= 30) { update_battery(); tick = 0; }
}

// ── Divider helper ───────────────────────────────────────────
static lv_obj_t * make_divider(lv_obj_t *parent, int width) {
    lv_obj_t *d = lv_obj_create(parent);
    lv_obj_set_size(d, width, 1);
    lv_obj_set_style_bg_color(d, CLR_GREEN_DBG, 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(d, 0, 0);
    lv_obj_set_style_pad_all(d, 0, 0);
    lv_obj_set_style_radius(d, 0, 0);
    return d;
}

// ── Screen create ────────────────────────────────────────────
lv_obj_t * start_screen_create(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, CLR_BLACK, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);

    // ── Title ────────────────────────────────────────────────
    static lv_style_t title_sty;
    lv_style_init(&title_sty);
    lv_style_set_text_font(&title_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&title_sty, CLR_GREEN_HI);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_add_style(title, &title_sty, 0);
    lv_label_set_text(title, "> ELEKNA <");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 38);

    lv_obj_t *div1 = make_divider(scr, 220);
    lv_obj_align(div1, LV_ALIGN_TOP_MID, 0, 72);

    // ── Time ─────────────────────────────────────────────────
    static lv_style_t time_sty;
    lv_style_init(&time_sty);
    lv_style_set_text_font(&time_sty, &lv_font_unscii_16);
    lv_style_set_text_color(&time_sty, CLR_GREEN_HI);

    time_lbl = lv_label_create(scr);
    lv_obj_add_style(time_lbl, &time_sty, 0);
    lv_label_set_text(time_lbl, "00:00:00");
    lv_obj_align(time_lbl, LV_ALIGN_CENTER, 0, -55);

    // ── Date ─────────────────────────────────────────────────
    static lv_style_t date_sty;
    lv_style_init(&date_sty);
    lv_style_set_text_font(&date_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&date_sty, CLR_GREEN_DIM);

    date_lbl = lv_label_create(scr);
    lv_obj_add_style(date_lbl, &date_sty, 0);
    lv_label_set_text(date_lbl, "MON  01 JAN 2000");
    lv_obj_align(date_lbl, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t *div2 = make_divider(scr, 220);
    lv_obj_align(div2, LV_ALIGN_CENTER, 0, -10);

    // ── WiFi row ─────────────────────────────────────────────
    wifi_img = lv_img_create(scr);
    lv_img_set_src(wifi_img, &Wifi_Icon);
    lv_obj_set_style_bg_opa(wifi_img, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wifi_img, 0, 0);
    lv_obj_set_style_pad_all(wifi_img, 0, 0);
    lv_obj_align(wifi_img, LV_ALIGN_CENTER, -70, 20);

    static lv_style_t status_sty;
    lv_style_init(&status_sty);
    lv_style_set_text_font(&status_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&status_sty, CLR_GREEN_DIM);

    wifi_lbl = lv_label_create(scr);
    lv_obj_add_style(wifi_lbl, &status_sty, 0);
    lv_label_set_text(wifi_lbl, "NO SIGNAL");
    lv_obj_align(wifi_lbl, LV_ALIGN_CENTER, 15, 20);

    // ── Battery row ──────────────────────────────────────────
    bat_img = lv_img_create(scr);
    lv_img_set_src(bat_img, &Battery_100_Icon);
    lv_obj_set_style_bg_opa(bat_img, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bat_img, 0, 0);
    lv_obj_set_style_pad_all(bat_img, 0, 0);
    lv_obj_align(bat_img, LV_ALIGN_CENTER, -70, 55);

    bat_lbl = lv_label_create(scr);
    lv_obj_add_style(bat_lbl, &status_sty, 0);
    lv_label_set_text(bat_lbl, "---%");
    lv_obj_align(bat_lbl, LV_ALIGN_CENTER, 15, 55);

    // ── Steps ────────────────────────────────────────────────
    static lv_style_t steps_sty;
    lv_style_init(&steps_sty);
    lv_style_set_text_font(&steps_sty, &lv_font_unscii_8);
    lv_style_set_text_color(&steps_sty, CLR_GREEN_DIM);

    steps_lbl = lv_label_create(scr);
    lv_obj_add_style(steps_lbl, &steps_sty, 0);
    lv_label_set_text(steps_lbl, "STEPS  00000");
    lv_obj_align(steps_lbl, LV_ALIGN_BOTTOM_MID, 0, -42);

    // ── Initial update ───────────────────────────────────────
    update_time_label();
    update_date_label();
    update_wifi_status_label();
    update_battery();
    update_steps();

    return scr;
}

void start_screen_enable_wifi_auto_update(void) {
    lv_timer_create(ui_timer_cb, 1000, NULL);
}

void start_screen_enable_battery_auto_update(void) { }
void start_screen_enable_ble_auto_update(void)     { }
