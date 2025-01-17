#pragma once
#include <BluetoothA2DPOutput.h>
#include <freertos/ringbuf.h>
#include "audio/SoundGenerator.h"
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
    Dropping,
};


class I2SRouter {
public:
    I2SRouter();

    bool begin();
    void end(); 

    uint32_t write_data_bt(Frame *data, uint32_t frame_count);
    uint32_t write_data_gen(Frame *data, uint32_t frame_count);
    SoundGenerator *generator = NULL;
    SoundGenerator *generator = NULL;

    void set_pin_config(i2s_pin_config_t pin_config)    { this->pin_config = pin_config; }
    void set_i2s_port(i2s_port_t i2s_num)               { i2s_port = i2s_num; }
    void set_i2s_config(i2s_config_t i2s_config)        { this->i2s_config = i2s_config; }

    void set_selected_output(OutputChoice output);

protected:
    friend void i2s_task_handler_(void *arg);

    friend void i2s_task_handler_(void *arg);

    volatile OutputChoice selected_output = OutputChoice::None;
    volatile int bt_skip_packets = 0;
    volatile int gen_skip_packets = 0;


    i2s_port_t i2s_port;
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;

    TaskHandle_t i2s_task_handle;
    void i2s_task_handler();

    size_t ringbuf_chunks_num = 20;
    size_t ringbuf_size = ringbuf_chunks_num * (128 * sizeof(Frame) + 8);
    RingbufHandle_t ringbuf_handle;
    volatile RingbufState ringbuf_state = RingbufState::Preloading;

    size_t inline ringbuf_fill_perc() {
        UBaseType_t items_waiting = 0;
        vRingbufferGetInfo(ringbuf_handle, NULL, NULL, NULL, NULL, &items_waiting);
        return 100 * items_waiting / ringbuf_chunks_num;
        // return 100 * ringbuf_size / (ringbuf_size - xRingbufferGetCurFreeSize(ringbuf_handle));

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

};