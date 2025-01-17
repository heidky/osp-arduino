#include <Arduino.h>
#include <BluetoothA2DPSink.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "config.h"
#include "ui/Display_ssd1306.h"
#include "state/AppManager.h"
#include "input/RotaryEncoder.h"
#include "input/Button.h"
#include "audio/AudioProcessor.h"
#include "audio/BTOutput.h"
#include "ble/BLEManager.h"
#include "audio/SoundGenerator.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
// #include <esp_log.h>

#define SERV_UUID "bdfd0000-00ed-4ef0-b809-9075032b21c2"
#define CHAR_UUID "bdfd0001-00ed-4ef0-b809-9075032b21c2"


// #define INTERNAL_DAC

Display_ssd1306 display;
TaskHandle_t Task_display;

BTOutput bt_output;
BluetoothA2DPSink a2dp_sink;

AppManager manager;
AudioProcessor audio_processor(&manager);
BLEManager ble(&manager);

SoundGenerator generator(&manager);
I2SRouter router;


RotaryEncoder l_encoder(GPIO_ROTARY_L_CLK, GPIO_ROTARY_L_DT);
Button l_button(GPIO_ROTARY_L_SW);
RotaryEncoder r_encoder(GPIO_ROTARY_R_CLK, GPIO_ROTARY_R_DT);
Button r_button(GPIO_ROTARY_R_SW);
Button m_button(GPIO_BUTTON_M);
long m_millis = -9999;


void TaskCode_display(void *) {
    for(;;) {
        if(manager.updated) {
            manager.updated = false;
            display.update(manager.state);
        }

        yield();
        delay(10);
    }
}


void bt_state_changed(esp_a2d_connection_state_t s, void*) {
    manager.set_bt_connected(s == ESP_A2D_CONNECTION_STATE_CONNECTED);

    // always start with 0 audio
    a2dp_sink.set_volume(0);

    Serial.print("connected: ");
    Serial.println(s == ESP_A2D_CONNECTION_STATE_CONNECTED);
}


void router_update_output() {
    const OutputChoice output = manager.state.source_mode == SOURCE_BT ? OutputChoice::Bt : OutputChoice::Gen;
    router.set_selected_output(output);
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
    xTaskCreatePinnedToCore(
        TaskCode_display,
        "Task_Display", 
        10000, 
        NULL, 
        0,
        &Task_display,
        0
    );

    // BT INIT
    a2dp_sink.set_output(bt_output);

    i2s_pin_config_t i2s_pin_config = {
        .bck_io_num = GPIO_I2S_BCK,
        .ws_io_num = GPIO_I2S_LCK,
        .data_out_num = GPIO_I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    // a2dp_sink.set_pin_config(i2s_pin_config);

    router.set_pin_config(i2s_pin_config);
    bt_output.router = &router;

    router.begin();
    router_update_output();

    // generator.start();
    router.generator = &generator;

#ifdef INTERNAL_DAC
    static const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = 44100, // corrected by info from bluetooth
        .bits_per_sample = (i2s_bits_per_sample_t) 16, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        // .tx_desc_auto_clear = false,
    };
    a2dp_sink.set_i2s_config(i2s_config);
#endif

    a2dp_sink.set_volume_control(&audio_processor);
    a2dp_sink.set_on_connection_state_changed(&bt_state_changed);
    a2dp_sink.set_volume(0);
    a2dp_sink.set_default_bt_mode(ESP_BT_MODE_BTDM);  // allows for BLE
    a2dp_sink.start(DEVICE_NAME);

    ble.init(DEVICE_NAME);

    manager.updated = true;
}   


void loop() {
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
        }
        else {
            manager.action(Action::DoublePressM);
        }

        m_millis = millis();
    }

    delay(1);
    yield();
}
 