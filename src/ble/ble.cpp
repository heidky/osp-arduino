#include "ble.h"

#include <esp_bt.h>            // ESP32 BLE
#include <esp_bt_device.h>     // ESP32 BLE
#include <esp_bt_main.h>       // ESP32 BLE
#include <esp_gap_ble_api.h>   // ESP32 BLE
#include <esp_gatts_api.h>     // ESP32 BLE
#include <esp_gattc_api.h>     // ESP32 BLE
#include <esp_gatt_common_api.h>// ESP32 BLE
#include <esp_err.h>           // ESP32 ESP-IDF

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// void ble::init()
// {
//     errRc = esp_ble_gap_register_callback(BLEDevice::gapEventHandler);
// 		if (errRc != ESP_OK) {
// 			ESP_LOGE(LOG_TAG, "esp_ble_gap_register_callback: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
// 			return;
// 		}
// }