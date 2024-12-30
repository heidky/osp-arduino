#include "BLEManager.h"
#include <BLEDevice.h>
#include <BLEServer.h>


// #define DEVICE_NAME_OVERRIDE "LVS-Z001 (Hush)"
// #define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
// #define CHAR_TX_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
// #define CHAR_RX_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

#define SERVICE_UUID "53300001-0023-4bd4-bbd5-a6920e4c5653"
#define CHAR_TX_UUID "53300002-0023-4bd4-bbd5-a6920e4c5653"
#define CHAR_RX_UUID "53300003-0023-4bd4-bbd5-a6920e4c5653"

#define TX_BUFF_SIZE 256

const static char *vibe_prefix = "Vibrate";


// todo: this should be a member of BLEServerCallbacks, BLECharacteristicCallbacks
static AppManager *s_manager;
static BLECharacteristic *s_rx_charasteric;


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    s_manager->set_ble_connected(true);
  };

  void onDisconnect(BLEServer* pServer) {
    s_manager->set_ble_connected(false);
    BLEDevice::startAdvertising();
    Serial.println("LVS BLE advertising!");
  }
};

class TxCharacteristicCallbacks: public BLECharacteristicCallbacks {
    char buff[TX_BUFF_SIZE] = {0};

    void onWrite(BLECharacteristic* pCharacteristic) {
        size_t len = pCharacteristic->getLength();
        const uint8_t* data = pCharacteristic->getData();   

        // copy buffer
        if (len > TX_BUFF_SIZE - 1) return; //too long
        strncpy(buff, (char*) data, len);
        buff[len] = '\0';
        
        // Serial.println(buff);

        // check commands
        if (strncmp(buff, "DeviceType;", len) == 0) {
            s_rx_charasteric->setValue("S:35:"); // reply device type
            s_rx_charasteric->notify();
        }
        else 
        if(strncmp(buff, "Status:1;", len) == 0) {
            s_rx_charasteric->setValue("2;"); // reply OK
            s_rx_charasteric->notify();
        } 
        else 
        if(strncmp(buff, "Battery;", len) == 0) {
            s_rx_charasteric->setValue("97;"); // reply full battery
            s_rx_charasteric->notify();
        }
        else {
            const char *col_pos = strchr(buff, ':');
            if(col_pos == NULL) return;

            const size_t prefix_len = strlen(vibe_prefix);

            if(strncmp(vibe_prefix, buff, prefix_len) == 0 && col_pos == (buff + prefix_len)) {
                char *end_ptr;
                long data = strtol(col_pos+1, &end_ptr, 10);

                if(col_pos+1 == end_ptr || *end_ptr != ';') return;
                if(data > 20 || data < 0) return;

                // Serial.println((int) data);
                s_manager->set_vibe_level((int) data);

                s_rx_charasteric->setValue("Ok;"); // reply full battery
                s_rx_charasteric->notify();
            }
        }
    }
};


// class MyCharCallbacks: public BLECharacteristicCallbacks {
//     void onWrite(BLECharacteristic* pCharacteristic) {
//         size_t len = pCharacteristic->getLength();
//         uint8_t* data = pCharacteristic->getData();

//         if(len > 0) {
//             uint8_t duck_level = data[0];
//             manager.set_duck_level(duck_level);
//         }
//     }
// };


BLEManager::BLEManager(AppManager *manager)
{
    if(manager == nullptr) while(true) delay(1000);
    this->manager = manager;
    s_manager = manager;
}

void BLEManager::init(const char *device_name)
{
    BLEDevice::init(device_name);
    // String mac_address = String(BLEDevice::getAddress().toString().c_str());
    // mac_address.replace(":", "");
    // mac_address.toUpperCase();
    // s_device_type = s_device_type + ":" + mac_address + ";";
    // Serial.println(s_device_type);

    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *rx_charasteric = pService->createCharacteristic(
                                            CHAR_RX_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_NOTIFY
                                            // BLECharacteristic::PROPERTY_INDICATE
                                        );
    s_rx_charasteric = rx_charasteric;
    rx_charasteric->addDescriptor(new BLEDescriptor(BLEUUID((uint16_t)0x2902)));
    rx_charasteric->setValue("");

    BLECharacteristic *tx_charasteric = pService->createCharacteristic(
                                            CHAR_TX_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE |
                                            BLECharacteristic::PROPERTY_WRITE_NR
                                        );
    tx_charasteric->addDescriptor(new BLEDescriptor(BLEUUID((uint16_t)0x2902)));
    tx_charasteric->setValue("");
    tx_charasteric->setCallbacks(new TxCharacteristicCallbacks());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);

    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();
    Serial.println("LVS BLE advertising!");
}


float BLEManager::vibe_level_to_mult(int level) {
    if(level <= 0) return 0.0f;
    else if (level >= VIBE_LEVEL_MAX) return 1.0f;

    switch (level) {
        case 1: return 0.2f; 
        case 2: return 0.4f; 
        case 3: return 0.5f; 
        case 4: return 0.6f; 
        default: break;
    }

    const float result = 0.6f + 0.4f * ((float)(level - 4) / (float)(VIBE_LEVEL_MAX - 4));
    return result > 1.f ? 1.f : (result < 0.f ? 0.f : result);
}
