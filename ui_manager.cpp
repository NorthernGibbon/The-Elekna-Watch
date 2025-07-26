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
}

void ui_show(Screen s) {
    printf("Switching to screen: %d\n", (int)s);
    current = s;
    lv_scr_load(screens[int(current)]);
}

void ui_handle_gesture(GESTURE g) {
  switch(g) {
    case SWIPE_LEFT:
      // advance to next screen
      current = Screen((int(current) + 1) % int(Screen::Count));
      ui_show(current);
      break;
    case SWIPE_RIGHT:
      // back to previous screen (wrap around)
      current = Screen((int(current) + int(Screen::Count) - 1) % int(Screen::Count));
      ui_show(current);
      break;
    default:
      // you can handle SWIPE_UP, SWIPE_DOWN, CLICK, etc. here too
      break;
  }
}