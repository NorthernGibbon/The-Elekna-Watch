#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <lvgl.h>
#include "Touch_CST816.h"  

enum class Screen {
  Home = 0,
  Clock = 1,
  Count
};

void ui_init();

void ui_show(Screen s);

void ui_handle_gesture(GESTURE g);

#endif // UI_MANAGER_H
