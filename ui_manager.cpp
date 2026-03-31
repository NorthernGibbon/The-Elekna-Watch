#include "ui_manager.h"
#include "start_screen.h"
#include "clock_screen.h"
#include "activity_screen.h"
#include "stopwatch_screen.h"
#include "Touch_CST816.h"

static lv_obj_t *screens[int(Screen::Count)];
static Screen    current = Screen::Home;

void ui_init() {
    screens[int(Screen::Home)]      = start_screen_create();
    screens[int(Screen::Clock)]     = clock_screen_create();
    screens[int(Screen::Activity)]  = activity_screen_create();
    screens[int(Screen::Stopwatch)] = stopwatch_screen_create();

    current = Screen::Home;
    lv_scr_load(screens[int(current)]);

    start_screen_enable_wifi_auto_update();
}

void ui_show(Screen s) {
    printf("Switching to screen: %d\n", (int)s);
    current = s;

    if (s == Screen::Activity) {
        activity_screen_refresh_history();
    }

    lv_scr_load(screens[int(current)]);
}

void ui_handle_gesture(GESTURE g) {
    switch (current) {

        // Home is the hub
        case Screen::Home:
            if      (g == SWIPE_LEFT)  ui_show(Screen::Clock);
            else if (g == SWIPE_RIGHT) ui_show(Screen::Activity);
            else if (g == SWIPE_UP)    ui_show(Screen::Stopwatch);
            break;

        // Clock: swipe right to go back
        case Screen::Clock:
            if (g == SWIPE_RIGHT) ui_show(Screen::Home);
            break;

        // Activity: swipe left to go back
        case Screen::Activity:
            if (g == SWIPE_LEFT) ui_show(Screen::Home);
            break;

        // Stopwatch: swipe down to go back
        case Screen::Stopwatch:
            if (g == SWIPE_DOWN) ui_show(Screen::Home);
            break;

        default:
            break;
    }
}

void ui_handle_tap(int raw_x, int raw_y) {
    if (current == Screen::Stopwatch) {
        stopwatch_handle_tap(raw_x, raw_y);
    }
}
