#include "Wireless.h"


bool WIFI_Connection = 0;
uint8_t WIFI_NUM = 0;
uint8_t BLE_NUM = 0;
bool Scan_finish = 0;
int wifi_scan_number()
{
  printf("/**********WiFi Test**********/\r\n");
  WiFi.mode(WIFI_STA);                           
  WiFi.setSleep(true);     
  int count = WiFi.scanNetworks();
  if (count == 0)
  {
    printf("No WIFI device was scanned\r\n");
  }
  else{
    printf("Scanned %d Wi-Fi devices\r\n",count);
  }
  
  WiFi.disconnect(true);
  WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);  
  vTaskDelay(100);           
  printf("/*******WiFi Test Over********/\r\n\r\n");
  return count;
}
int ble_scan_number()
{
  printf("/**********BLE Test**********/\r\n"); 
  BLEDevice::init("ESP32");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);

  BLEScanResults* foundDevices = pBLEScan->start(5);  
  int count = foundDevices->getCount();
  if (count == 0)
  {
    printf("No Bluetooth device was scanned\r\n");
  }
  else{
    printf("Scanned %d Bluetooth devices\r\n",count);
  }
  pBLEScan->stop(); 
  pBLEScan->clearResults(); 
  BLEDevice::deinit(true); 
  vTaskDelay(100);         
  printf("/**********BLE Test Over**********/\r\n\r\n");
  return count;
}
extern char buffer[128];   
void Wireless_Test1(){                    
  WIFI_NUM = wifi_scan_number();
  Scan_finish = 1;
}

void WirelessScanTask(void *parameter) {                  
  WIFI_NUM = wifi_scan_number();
  Scan_finish = 1;
  vTaskDelay(pdMS_TO_TICKS(1000));
  vTaskDelete(NULL);
}
void Wireless_Test2(){
  xTaskCreatePinnedToCore(
    WirelessScanTask,    
    "WirelessScanTask",   
    4096,                
    NULL,                 
    2,                   
    NULL,                 
    0                   
  );
}
