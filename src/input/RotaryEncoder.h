#pragma once

#include <Arduino.h>

class RotaryEncoder {
protected:
    uint8_t clk_pin;
    uint8_t dt_pin;

public:
    bool reverse;
    bool changed;
    RotaryEncoder(uint8_t clk_pin, uint8_t dt_pin);

    void begin();

    int read();
};
