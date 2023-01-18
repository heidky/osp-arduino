#pragma once

#include <Arduino.h>

class Button {
protected:
    uint8_t pin;
    int prev = 1;
    long last_millis = -99999;

public:
    bool changed;

    Button(uint8_t pin);

    void begin();

    int read();
};