#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <lvgl.h>
#include "Touch_CST816.h"   // for GESTURE enum

/// Your two screens
enum class Screen {
  Home = 0,
  Clock = 1,
  Count  // always last: number of screens
};

/// Build screens & load the default
void ui_init();

/// Immediately switch to screen `s`
void ui_show(Screen s);

/// Call this whenever you detect a CST816 gesture
void ui_handle_gesture(GESTURE g);

#endif // UI_MANAGER_H
