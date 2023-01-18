#include "RotaryEncoder.h"
#include "config.h"

RotaryEncoder::RotaryEncoder(uint8_t clk_pin, uint8_t dt_pin) {
    this->clk_pin = clk_pin;
    this->dt_pin = dt_pin;
}

static void IRAM_ATTR button_isr(void* arg) {
    RotaryEncoder* r = static_cast<RotaryEncoder*>(arg);
    r->changed = true;
}

void RotaryEncoder::begin() {
    pinMode(clk_pin, INPUT_PULLUP);
    pinMode(dt_pin, INPUT_PULLUP);

    attachInterruptArg(clk_pin, &button_isr, this, FALLING);
}

int RotaryEncoder::read() {
    int result = 0;

    if(changed) {
        int dt_value = digitalRead(dt_pin);
        delay(BTN_DEBOUNCE_MILLIS);

        if(digitalRead(clk_pin) == 0) {
            if(dt_value == 0) 
                result = -1;
            else
                result = 1;

            if(reverse)
                result *= -1;
        }

        changed = false;
    }
        
    return result;
}