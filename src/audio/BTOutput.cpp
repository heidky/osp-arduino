#include "BTOutput.h"
#include "Arduino.h"


bool BtOutput::begin() {
    return true;
}

void BtOutput::end() {}

size_t BtOutput::write(const uint8_t *data, size_t len) {   
    router->on_bt_data((Frame*) data, len / sizeof(Frame));
    return len;
}

void BtOutput::set_sample_rate(int rate) {
    sample_rate = rate;
    router->set_bt_sample_rate(rate);

    Serial.print("sample rate: ");
    Serial.println(rate);
}

void BtOutput::set_output_active(bool active) {
    active = active;
    router->set_bt_active(active);

    Serial.print("output active: ");
    Serial.println(active);
}
