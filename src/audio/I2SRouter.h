#pragma once
#include <BluetoothA2DPOutput.h>
#include <freertos/ringbuf.h>
#include "audio/SoundGenerator.h"

enum class OutputChoice {
    None,
    Bt,
    Gen,
};


enum class RingbufState {
    Preloading,
    Processing,
};


class I2SRouter {
public:
    I2SRouter();

    bool begin();
    void end(); 

    uint32_t write_data_bt(Frame *data, uint32_t frame_count);
    uint32_t write_data_gen(Frame *data, uint32_t frame_count);
    SoundGenerator *generator = NULL;

    void set_pin_config(i2s_pin_config_t pin_config)    { this->pin_config = pin_config; }
    void set_i2s_port(i2s_port_t i2s_num)               { i2s_port = i2s_num; }
    void set_i2s_config(i2s_config_t i2s_config)        { this->i2s_config = i2s_config; }

    void set_selected_output(OutputChoice output);

protected:
    friend void i2s_task_handler_(void *arg);

    volatile OutputChoice selected_output = OutputChoice::None;
    volatile int bt_skip_packets = 0;
    volatile int gen_skip_packets = 0;


    i2s_port_t i2s_port;
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;

    TaskHandle_t i2s_task_handle;
    void i2s_task_handler();

    RingbufHandle_t ringbuf_handle;
    size_t ringbuf_size;
    volatile RingbufState ringbuf_state = RingbufState::Preloading;

    size_t inline ringbuf_fill_perc() {
        return 100 * ringbuf_size / (ringbuf_size - xRingbufferGetCurFreeSize(ringbuf_handle));
    }

    void inline clear_ringbuf() {
        size_t item_size;
        void *data;
        while((data = xRingbufferReceive(ringbuf_handle, &item_size, 0)) != NULL) {
            vRingbufferReturnItem(ringbuf_handle, data);
        }
    }

};