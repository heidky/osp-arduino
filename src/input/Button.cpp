#include "Button.h"
#include "config.h"

Button::Button(uint8_t pin) {
    this->pin = pin;
}

static void IRAM_ATTR button_isr(void* arg) {
    Button* b = static_cast<Button*>(arg);
    b->changed = true;
}

void Button::begin() {
    pinMode(pin, INPUT_PULLUP);

    // attachInterruptArg(pin, &button_isr, this, FALLING);
}

int Button::read() {
    int result = 0;
    int current = digitalRead(pin);

    if(prev != current) {
        if(millis() - last_millis <= BTN_SLEEP_MILLIS) return 0;

        if(current == 0) {
            result = 1;
            last_millis = millis();
        }

        prev = current;
    }

    return result;
}

// int Button::read() {
//     int result = 0;

//     if(changed) {
//         delay(10);

//         if(digitalRead(pin) == 0) {
//             result = 1;
//         }

//         changed = false;
//     }

//     return result;
// }