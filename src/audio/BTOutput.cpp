#include "BTOutput.h"
#include "Arduino.h"

// void BTOutput::begin() {}
// void BTOutput::end() {}


size_t BTOutput::write(const uint8_t *data, size_t len) {
    return len;
}

void BTOutput::set_sample_rate(int rate) {
    Serial.print("sample rate: ");
    Serial.println(rate);
}

void BTOutput::set_output_active(bool active) {
    Serial.print("output active: ");
    Serial.println(active);
}
