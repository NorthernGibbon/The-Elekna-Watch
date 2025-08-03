#include "bluetooth_manager.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define BLE_DEVICE_NAME           "EleknaWatch"
#define BLE_SERVICE_UUID          "12345678-1234-1234-1234-1234567890ab"
#define BLE_CHARACTERISTIC_UUID   "abcdefab-1234-5678-1234-abcdefabcdef"

static BLEServer* pServer = nullptr;
static BLECharacteristic* pCharacteristic = nullptr;
static bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("BLE Device connected!");
    }
    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("BLE Device disconnected!");
        pServer->getAdvertising()->start();
    }
};

void bluetooth_init(void) {
    BLEDevice::init(BLE_DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(BLE_SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        BLE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->setValue("Hello from EleknaWatch!");
    pService->start();

    pServer->getAdvertising()->start();
    Serial.println("BLE advertising started.");
}

bool bluetooth_is_connected(void) {
    return deviceConnected; 
}

void bluetooth_notify(const String& msg) {
    if (deviceConnected && pCharacteristic) {
        pCharacteristic->setValue(msg); 
        pCharacteristic->notify();
        Serial.println("Notification sent via BLE: " + msg);
    }
}

