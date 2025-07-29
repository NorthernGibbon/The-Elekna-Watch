#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <Arduino.h>

// Start the BLE server/advertising
void bluetooth_init(void);

// Check if a BLE device (phone) is currently connected
bool bluetooth_is_connected(void);

// Send a notification (string message) to the phone
void bluetooth_notify(const String& msg);


#endif // BLUETOOTH_MANAGER_H
