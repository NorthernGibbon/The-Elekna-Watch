#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

enum WifiStatus {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_FAILED
};

// Call this once in setup to connect
void wifi_manager_connect(const char* ssid, const char* password);

// Call this in your LVGL timer or loop to check status
WifiStatus wifi_manager_get_status(void);

// Get the IP as a String (after connected)
String wifi_manager_get_ip(void);

#endif // WIFI_MANAGER_H
