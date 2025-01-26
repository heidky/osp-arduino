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
    I2SRouter(AppManager *manager);

    bool begin();
    void end(); 

    size_t on_bt_data(Frame *data, size_t frame_count);

    void set_pin_config(i2s_pin_config_t pin_config)    { this->pin_config = pin_config; }
    void set_i2s_port(i2s_port_t i2s_num)               { this->i2s_port = i2s_num; }
    void set_i2s_config(i2s_config_t i2s_config)        { this->i2s_config = i2s_config; }

    void set_selected_output(OutputChoice output);

    void set_bt_sample_rate(int32_t rate);
    void set_bt_active(bool active);

    inline void get_meter(float *out_l, float *out_r) {
        *out_l = common_processing.meter_l.value;
        *out_r = common_processing.meter_r.value;
    }

    inline void reset_meter() {
        common_processing.meter_l.value = 0;
        common_processing.meter_r.value = 0;
    }

protected:
    friend void i2s_task_handler_(void *arg);

    AppManager *manager = nullptr;
    SoundGenerator generator;

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
    SemaphoreHandle_t ringbuf_clearing;
    volatile bool ringbuf_needs_clearing = false;

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

    CommonProcessing common_processing;
    BtProcessing bt_processing;

    void set_sample_rate(int32_t sample_rate);

    volatile int32_t bt_sample_rate = 44100;
    volatile bool bt_active = true;
    volatile bool bt_sample_rate_changed = false;
    SemaphoreHandle_t bt_sample_rate_mutex = NULL;

    volatile bool generator_sample_rate_changed = false;
};
