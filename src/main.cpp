#include <Arduino.h>
#include <BluetoothA2DPSink.h>
// #include <BluetoothA2DPSinkQueued.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "config.h"
#include "ui/Display_ssd1306.h"
#include "state/AppManager.h"
#include "input/RotaryEncoder.h"
#include "input/Button.h"
#include "audio/BtVolumeControl.h"
#include "audio/BTOutput.h"
#include "ble/BLEManager.h"
#include "audio/SoundGenerator.h"
// #include <esp_log.h>

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

AppManager manager;

// DISPLAY
Display_ssd1306 display;
TaskHandle_t Task_display;

// OUTPUT
I2SRouter router;
SoundGenerator generator(&manager);

// BT
BluetoothA2DPSink a2dp_sink;
BtOutput bt_output;
BtVolumeControl bt_volume_control;
// BluetoothA2DPSinkQueued a2dp_sink;
BLEManager ble(&manager);

// INPUTS
RotaryEncoder l_encoder(GPIO_ROTARY_L_CLK, GPIO_ROTARY_L_DT);
Button l_button(GPIO_ROTARY_L_SW);
RotaryEncoder r_encoder(GPIO_ROTARY_R_CLK, GPIO_ROTARY_R_DT);
Button r_button(GPIO_ROTARY_R_SW);
Button m_button(GPIO_BUTTON_M);


void TaskCode_display(void *) {
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        State state;
        if(manager.copy_state_into(state)) {
            display.update(state, manager.ui_state);
        }

        yield();
        delay(10);
    }
}


void bt_state_changed(esp_a2d_connection_state_t s, void*) {
    manager.set_bt_connected_(s == ESP_A2D_CONNECTION_STATE_CONNECTED);

    // always start with 0 audio
    a2dp_sink.set_volume(0);

    Serial.print("connected: ");
    Serial.println(s == ESP_A2D_CONNECTION_STATE_CONNECTED);
}


void router_update_output() {
    State state;
    if(manager.copy_state_into(state)) {
        bt_volume_control.set_muted(state.source_mode != SOURCE_BT);
        router.set_selected_output(state.source_mode == SOURCE_BT ? OutputChoice::Bt : OutputChoice::Gen);
    }
}


void setup() {
    Serial.begin(115200);
    Serial.println("\nOsp start");

    Wire.setPins(GPIO_I2C_SDA, GPIO_I2C_SCL);
    // init display
    display.begin();

    // init controls
    l_encoder.begin();
    l_button.begin();
    r_encoder.begin();
    r_button.begin();
    m_button.begin();

    // show splashscreen
    display.splash();
    // create display refresh task
    xTaskCreatePinnedToCore(TaskCode_display, "Task_Display", 2048, NULL, 0, &Task_display, 0);

    // BT INIT
    a2dp_sink.set_output(bt_output);

    i2s_pin_config_t i2s_pin_config = {
        .bck_io_num = GPIO_I2S_BCK,
        .ws_io_num = GPIO_I2S_LCK,
        .data_out_num = GPIO_I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    router.set_pin_config(i2s_pin_config);
    router.manager = &manager;
    bt_output.router = &router;

    generator.begin();
    router.generator = &generator;

    router.begin();
    router_update_output();

    a2dp_sink.set_volume_control(&bt_volume_control);
    a2dp_sink.set_on_connection_state_changed(&bt_state_changed);
    a2dp_sink.set_volume(0);
    a2dp_sink.set_default_bt_mode(ESP_BT_MODE_BTDM);  // allows for BLE
    a2dp_sink.start(DEVICE_NAME);

    ble.init(DEVICE_NAME);

    manager.updated = true;
}   


void loop() {
    static uint32_t loop_count = 0;
    static long m_millis = -10000;

    int l_value = l_encoder.read();
    int r_value = r_encoder.read();

    if(l_value != 0) {
        manager.action(Action::ValueL, &l_value);
    }

    if(r_value != 0) {
        manager.action(Action::ValueR, &r_value);
    }

    if(l_button.read() == 1) {
        manager.action(Action::PressL);
    }

    if(r_button.read() == 1) {
        manager.action(Action::PressR);
    }

    if(m_button.read() == 1) {
        if(millis() - m_millis >= BTN_DOUBLE_PRESS_MILLIS) {
            manager.action(Action::PressM);
            router_update_output(); // only does stuff if needed
            m_millis = millis();
        }
        else {
            manager.action(Action::DoublePressM);
            m_millis = -10000;
        }
    }

    if(loop_count % 10 == 0) {
        router.get_meter(&manager.ui_state.l_level, &manager.ui_state.r_level);
        manager.updated = true;
    }

    if(manager.updated) {
        manager.updated = false;
        xTaskNotifyGive(Task_display);
    }

    loop_count++;
    delay(1);
    yield();
}
 