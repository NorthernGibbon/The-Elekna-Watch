#include "start_screen.h"
#include "wifi_manager.h"
#include <lvgl.h>
#include <WiFi.h>      // Still ok for basic functions, but not needed for status


static WifiStatus current_status = WIFI_DISCONNECTED;

void wifi_manager_connect(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);

    current_status = WIFI_CONNECTING;
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 20) {
        delay(500);
        Serial.print(".");
        tries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
        current_status = WIFI_CONNECTED;
    } else {
        Serial.println("\nFailed to connect. Status: " + String(WiFi.status()));
        current_status = WIFI_FAILED;
    }
}

WifiStatus wifi_manager_get_status(void) {
    // Update status live (in case WiFi drops out after initial connect)
    if (WiFi.status() == WL_CONNECTED) {
        current_status = WIFI_CONNECTED;
    } else if (current_status == WIFI_CONNECTING) {
        // Stay as connecting
    } else if (current_status == WIFI_CONNECTED) {
        current_status = WIFI_DISCONNECTED;
    }
    return current_status;
}

String wifi_manager_get_ip(void) {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    } else {
        return String("0.0.0.0");
    }
}
