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
#include "audio/AudioProcessor.h"
#include "ble/BLEManager.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
// #include <esp_log.h>
// #define INTERNAL_DAC

Display_ssd1306 display;
BluetoothA2DPSink a2dp_sink;
// BluetoothA2DPSinkQueued a2dp_sink;
TaskHandle_t Task_display;

AppManager manager;
AudioProcessor audio_processor(&manager);
BLEManager ble(&manager);


RotaryEncoder l_encoder(GPIO_ROTARY_L_CLK, GPIO_ROTARY_L_DT);
Button l_button(GPIO_ROTARY_L_SW);
RotaryEncoder r_encoder(GPIO_ROTARY_R_CLK, GPIO_ROTARY_R_DT);
Button r_button(GPIO_ROTARY_R_SW);
Button m_button(GPIO_BUTTON_M);


void TaskCode_display(void *) {
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        display.update(manager.state, manager.ui_state);

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
    i2s_pin_config_t i2s_pin_config = {
        .bck_io_num = GPIO_I2S_BCK,
        .ws_io_num = GPIO_I2S_LCK,
        .data_out_num = GPIO_I2S_DOUT,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    a2dp_sink.set_pin_config(i2s_pin_config);

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
#else
    // setup default i2s config
    static const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,  // updated by a2dp_sink when needed
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,  // default interrupt priority
        // .dma_buf_count = 8,
        // .dma_buf_len = 64,
        .dma_buf_count = 4, // bigger buffer to accomodate a full bt data transmission
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,  // avoiding noise in case of data unavailability
        .fixed_mclk = 0,
        .mclk_multiple = (i2s_mclk_multiple_t) 0, // I2S_MCLK_MULTIPLE_DEFAULT
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
    };
#endif
    a2dp_sink.set_i2s_config(i2s_config);

    a2dp_sink.set_volume_control(&audio_processor);
    a2dp_sink.set_on_connection_state_changed(&bt_state_changed);
    a2dp_sink.set_volume(0);
    a2dp_sink.set_default_bt_mode(ESP_BT_MODE_BTDM);  // allows for BLE
    a2dp_sink.start(DEVICE_NAME);

    ble.init(DEVICE_NAME);

    manager.updated = true;
}   


void loop() {
    static uint32_t loop_count = 0;
    static long m_millis = -9999;

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
        }
        else {
            manager.action(Action::DoublePressM);
        }

        m_millis = millis();
    }

    if(loop_count % 10 == 0) {
        manager.ui_state.l_level = audio_processor.meter_l.value;
        manager.ui_state.r_level = audio_processor.meter_r.value;
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
