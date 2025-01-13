#include "I2SRouter.h"
#include <Arduino.h>

I2SRouter::I2SRouter() {
    // default i2s port is 0
    i2s_port = (i2s_port_t)0;

    // setup default i2s config
    i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,  // default interrupt priority
        .dma_buf_count = 4,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,  // avoiding noise in case of data unavailability
        .fixed_mclk = 0,
        .mclk_multiple = (i2s_mclk_multiple_t) 0, // I2S_MCLK_MULTIPLE_DEFAULT
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
    };

    // setup default pins
    pin_config = {
        .mck_io_num = 0,
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
}

bool I2SRouter::begin() {
    bool rc = true;
    
    if (i2s_driver_install(i2s_port, &i2s_config, 0, NULL) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "i2s_driver_install failed");
        rc = false;
    }

    if (i2s_set_pin(i2s_port, &pin_config) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "i2s_set_pin failed");
        rc = false;
        
    }

  return rc;
}

void I2SRouter::end() {// TODO 
}

uint32_t I2SRouter::write_data_bt(Frame *data, uint32_t frame_count) {
    if(selected_output != OutputChoice::Bt) return frame_count;
    if(--bt_skip_packets > 0) return frame_count;
    if(bt_skip_packets == 0) i2s_zero_dma_buffer(i2s_port);

    size_t item_size = frame_count * sizeof(Frame);
    size_t i2s_bytes_written = 0;

    if (i2s_write(i2s_port, (void *)data, item_size, &i2s_bytes_written, portMAX_DELAY) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "i2s_write has failed");
    }
    return i2s_bytes_written / sizeof(Frame);
}

uint32_t I2SRouter::write_data_gen(Frame *data, uint32_t frame_count) {
    if(selected_output != OutputChoice::Gen) return frame_count;
    if(--gen_skip_packets > 0) return frame_count;

    size_t item_size = frame_count * sizeof(Frame);
    size_t i2s_bytes_written = 0;

    if (i2s_write(i2s_port, (void *)data, item_size, &i2s_bytes_written, portMAX_DELAY) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "i2s_write has failed");
    }
    return i2s_bytes_written / sizeof(Frame);
}

void I2SRouter::set_selected_output(OutputChoice new_output) {
    if(new_output == this->selected_output) return;

    if(new_output == OutputChoice::Bt) bt_skip_packets = 16;
    else if(new_output == OutputChoice::Gen) gen_skip_packets = 16;
    this->selected_output = new_output;

    Serial.println("switching routed output");
}
