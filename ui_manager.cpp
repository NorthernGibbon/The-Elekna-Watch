#include "ui_manager.h"
#include "start_screen.h"
#include "clock_screen.h"

static lv_obj_t * screens[int(Screen::Count)];
static Screen      current = Screen::Home;

void ui_init() {
  // create both screens
  screens[int(Screen::Home)]  = start_screen_create();
  screens[int(Screen::Clock)] = clock_screen_create();

  current = Screen::Home;
  lv_scr_load(screens[int(current)]);

  start_screen_enable_wifi_auto_update();
}

void ui_show(Screen s) {
    printf("Switching to screen: %d\n", (int)s);
    current = s;
    lv_scr_load(screens[int(current)]);
}

void ui_handle_gesture(GESTURE g) {
    switch (g) {
        case SWIPE_LEFT:
            // Next screen
            current = Screen((int(current) + 1) % int(Screen::Count));
            lv_scr_load_anim(screens[int(current)],
                             LV_SCR_LOAD_ANIM_MOVE_LEFT, 
                             150,                        
                             0,                          
                             false);                     
            break;

        case SWIPE_RIGHT:
            // Previous screen
            current = Screen((int(current) + int(Screen::Count) - 1) % int(Screen::Count));
            lv_scr_load_anim(screens[int(current)],
                             LV_SCR_LOAD_ANIM_MOVE_RIGHT,
                             150,
                             0,
                             false);
            break;

        default:
            // Handle SWIPE_UP, SWIPE_DOWN, CLICK, etc. here if you want
            break;
    }
}

