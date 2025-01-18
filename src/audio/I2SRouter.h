#pragma once
#include <BluetoothA2DPOutput.h>
#include <freertos/ringbuf.h>
#include "audio/SoundGenerator.h"
#include "audio/processing/CommonProcessing.h"
#include "audio/processing/BtProcessing.h"

enum class OutputChoice {
    None,
    Bt,
    Gen,
};

enum class RingbufState {
    Preloading,
    Processing,
    Dropping,
};

class I2SRouter {
public:
    I2SRouter();

    bool begin();
    void end(); 

    AppManager *manager = nullptr;

    SoundGenerator *generator = nullptr;
    size_t on_bt_data(Frame *data, size_t frame_count);

    void set_pin_config(i2s_pin_config_t pin_config)    { this->pin_config = pin_config; }
    void set_i2s_port(i2s_port_t i2s_num)               { i2s_port = i2s_num; }
    void set_i2s_config(i2s_config_t i2s_config)        { this->i2s_config = i2s_config; }

    void set_selected_output(OutputChoice output);

protected:
    friend void i2s_task_handler_(void *arg);

    i2s_port_t i2s_port;
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;

    volatile OutputChoice selected_output = OutputChoice::None;

    TaskHandle_t i2s_task_handle;
    void i2s_task_handler();

    RingbufHandle_t ringbuf_handle;
    volatile RingbufState ringbuf_state = RingbufState::Preloading;
    size_t ringbuf_chunks_num = 20;
    size_t ringbuf_size = ringbuf_chunks_num * (128 * sizeof(Frame) + 8);

    size_t inline ringbuf_fill_perc() {
        UBaseType_t items_waiting = 0;
        vRingbufferGetInfo(ringbuf_handle, NULL, NULL, NULL, NULL, &items_waiting);
        return 100 * items_waiting / ringbuf_chunks_num;
    }

    size_t inline ringbuf_items_waiting() {
        UBaseType_t items_waiting = 0;
        vRingbufferGetInfo(ringbuf_handle, NULL, NULL, NULL, NULL, &items_waiting);
        return items_waiting;
    }

    void inline clear_ringbuf() {
        size_t item_size;
        void *data;
        while((data = xRingbufferReceive(ringbuf_handle, &item_size, 0)) != NULL) {
            vRingbufferReturnItem(ringbuf_handle, data);
        }
    }

    CommonProcessing common_processing;
    BtProcessing bt_processing;

    void set_sample_rate(uint32_t sample_rate);
};
