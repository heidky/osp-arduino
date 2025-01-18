#include "BTOutput.h"
#include "Arduino.h"


bool BTOutput::begin() {
    return true;
}

void BTOutput::end() {}

size_t BTOutput::write(const uint8_t *data, size_t len) {   
    router->on_bt_data((Frame*) data, len / sizeof(Frame));
    return len;
}

void BTOutput::set_sample_rate(int rate) {
    Serial.print("sample rate: ");
    Serial.println(rate);
    samplerate = rate;
}

void BTOutput::set_output_active(bool active) {
    Serial.print("output active: ");
    Serial.println(active);
    active = active;
}
