#pragma once
#include <BluetoothA2DPOutput.h>


enum class OutputChoice {
    None,
    Bt,
    Gen,
};


class I2SRouter {
public:
    I2SRouter();

    bool begin();
    void end(); 

    uint32_t write_data_bt(Frame *data, uint32_t frame_count);
    uint32_t write_data_gen(Frame *data, uint32_t frame_count);

    void set_pin_config(i2s_pin_config_t pin_config)    { this->pin_config = pin_config; }
    void set_i2s_port(i2s_port_t i2s_num)               { i2s_port = i2s_num; }
    void set_i2s_config(i2s_config_t i2s_config)        { this->i2s_config = i2s_config; }

    void set_selected_output(OutputChoice output);

protected:
    volatile OutputChoice selected_output = OutputChoice::None;
    volatile int bt_skip_packets = 0;
    volatile int gen_skip_packets = 0;


    i2s_port_t i2s_port;
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;
};