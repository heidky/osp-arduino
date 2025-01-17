#include "I2SRouter.h"
#include <Arduino.h>

#define I2SR_TAG "I2S_ROUTER"

void i2s_task_handler_(void *arg) {   
    I2SRouter *router = (I2SRouter*)arg;
    router->i2s_task_handler();
}

#define I2SR_TAG "I2S_ROUTER"

void i2s_task_handler_(void *arg) {   
    I2SRouter *router = (I2SRouter*)arg;
    router->i2s_task_handler();
}

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
        .dma_buf_count = 2,
        .dma_buf_len = 128,
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

    ringbuf_size = 16 * (128 * sizeof(Frame) + 8);
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

    ringbuf_handle = xRingbufferCreate(ringbuf_size, RINGBUF_TYPE_NOSPLIT);
    if (ringbuf_handle == NULL) {
        ESP_LOGE(BT_AV_TAG, "ringbuff creation failed");
        rc = false;
    }

    const BaseType_t priority = 5;
    const BaseType_t core = 1;
    const uint32_t stack_size = 2048;

    BaseType_t result = xTaskCreatePinnedToCore(i2s_task_handler_, "I2S router", stack_size, (void*)this, priority, &i2s_task_handle, core);
    if (result != pdPASS)   {
        ESP_LOGE(I2SR_TAG, "cannot create i2s router task");
        rc = false;
    }

    return rc;
}

void I2SRouter::end() {// TODO 
}

uint32_t I2SRouter::write_data_bt(Frame *data, uint32_t frame_count) {
    if(selected_output != OutputChoice::Bt) {
        return frame_count;
    }

    // static long last_call_millis = -9999;
    // long now_millis = millis();
    // if(last_call_millis > 0) Serial.println(now_millis - last_call_millis);
    // last_call_millis = now_millis;

    if(ringbuf_state == RingbufState::Dropping) {
        return 0;
    }

    uint32_t written_frames = 0;
    const uint32_t FRAMES_PER_CHUNK = 128;
    const size_t write_timeout_ms = 6;

    while(written_frames < frame_count) {
        const Frame *data_to_write = (data + written_frames);
        const uint32_t remaining_frames = frame_count - written_frames;
        const uint32_t frames_to_write = remaining_frames > FRAMES_PER_CHUNK ? FRAMES_PER_CHUNK : remaining_frames;

        if(frames_to_write != FRAMES_PER_CHUNK) ESP_LOGE(BT_AV_TAG, "bad chunk size");

        const BaseType_t res = xRingbufferSend(ringbuf_handle, (void*)data_to_write, frames_to_write * sizeof(Frame), pdMS_TO_TICKS(write_timeout_ms));
        if(res == pdTRUE) {
            written_frames += frames_to_write;
        } else {
            Serial.println("ringbuffer full. enter dropping");
            ringbuf_state = RingbufState::Dropping;
            return written_frames;
        }
    }

    return frame_count;

    // if(selected_output != OutputChoice::Bt) return frame_count;
    // if(--bt_skip_packets > 0) return frame_count;
    // if(bt_skip_packets == 0) i2s_zero_dma_buffer(i2s_port);

    // size_t item_size = frame_count * sizeof(Frame);
    // size_t i2s_bytes_written = 0;
    // size_t item_size = frame_count * sizeof(Frame);
    // size_t i2s_bytes_written = 0;

    // if (i2s_write(i2s_port, (void *)data, item_size, &i2s_bytes_written, portMAX_DELAY) != ESP_OK) {
    //     ESP_LOGE(BT_AV_TAG, "i2s_write has failed");
    // }
    // return i2s_bytes_written / sizeof(Frame);
    // if (i2s_write(i2s_port, (void *)data, item_size, &i2s_bytes_written, portMAX_DELAY) != ESP_OK) {
    //     ESP_LOGE(BT_AV_TAG, "i2s_write has failed");
    // }
    // return i2s_bytes_written / sizeof(Frame);
}

uint32_t I2SRouter::write_data_gen(Frame *data, uint32_t frame_count) {
    Serial.println("Why here?");
    // if(selected_output != OutputChoice::Gen) return frame_count;
    // if(--gen_skip_packets > 0) return frame_count;
    Serial.println("Why here?");
    // if(selected_output != OutputChoice::Gen) return frame_count;
    // if(--gen_skip_packets > 0) return frame_count;

    // size_t item_size = frame_count * sizeof(Frame);
    // size_t i2s_bytes_written = 0;

    // if (i2s_write(i2s_port, (void *)data, item_size, &i2s_bytes_written, portMAX_DELAY) != ESP_OK) {
    //     ESP_LOGE(BT_AV_TAG, "i2s_write has failed");
    // }
    // return i2s_bytes_written / sizeof(Frame);
    return frame_count;
}

void I2SRouter::set_selected_output(OutputChoice new_output) {
    if(new_output == this->selected_output) return;

    if(new_output == OutputChoice::Bt) {
        // bt_skip_packets = 16;

        clear_ringbuf();
        ringbuf_state = RingbufState::Preloading; 
        this->selected_output = new_output;
        Serial.println("switched routed output to Bt");
    }
    else if(new_output == OutputChoice::Gen) {
        // gen_skip_packets = 16;
        this->selected_output = new_output;
        clear_ringbuf();
        Serial.println("switching routed output to Gen");
    }
}

void I2SRouter::i2s_task_handler() {

    static const size_t RINGBUF_PRELOAD_PERC = 50;
    static const size_t RINGBUF_PRELOAD_ITEMS = 10;
    static const BaseType_t READ_TIMEOUT_MS = 5;

    for (;;) 
    {    
        if(selected_output == OutputChoice::Bt) 
        {
            if (ringbuf_state == RingbufState::Preloading) {
                // if(ringbuf_fill_perc() > RINGBUF_PRELOAD_PERC) {
                if(ringbuf_items_waiting() >= RINGBUF_PRELOAD_ITEMS) {
                    ringbuf_state = RingbufState::Processing; 
                    // Serial.print("into processing: ");
                    // Serial.println(ringbuf_fill_perc());
                }
            }
            else if (ringbuf_state == RingbufState::Dropping) {
                size_t item_size, fill_perc = 0;

                // while((fill_perc = ringbuf_fill_perc()) > RINGBUF_PRELOAD_PERC) {
                while(ringbuf_items_waiting() > RINGBUF_PRELOAD_ITEMS) {
                    void *item = xRingbufferReceive(ringbuf_handle, &item_size, 0);
                    vRingbufferReturnItem(ringbuf_handle, item);
                }

                Serial.print("dropped under water level: ");
                Serial.println(fill_perc);
                ringbuf_state = RingbufState::Preloading;
            }

            if (ringbuf_state == RingbufState::Processing) {
                size_t item_size = 0;
                void *item = xRingbufferReceive(ringbuf_handle, &item_size, pdMS_TO_TICKS(READ_TIMEOUT_MS));

                if(item != NULL && item_size > 0) {
                    size_t bytes_written;
                    i2s_write(i2s_port, item, item_size, &bytes_written, portMAX_DELAY);
                    vRingbufferReturnItem(ringbuf_handle, item);
                }
                else {
                    Serial.println("data starvation!");
                    // Serial.println(ringbuf_fill_perc());
                    ringbuf_state = RingbufState::Preloading;
                }
            }
            else {
                delay(1);
            }
        }
        else if (selected_output == OutputChoice::Gen) 
        {
            const size_t chunk_size = 128;
            const void *data = generator->receive_chunk(chunk_size);

            if(data != NULL) {
                size_t bytes_written = 0;
                const size_t size = chunk_size * sizeof(Frame);
                i2s_write(i2s_port, data, size, &bytes_written, portMAX_DELAY);
            }
        }

        delay(1);
    }
}


