#include "RouterOutput.h"
#include <Arduino.h>
#include "logging.h"


bool RouterOutput::begin() {
    return true;
}

void RouterOutput::end() {}

size_t RouterOutput::write(const uint8_t *data, size_t len) {   
    router->on_bt_data((Frame*) data, len / sizeof(Frame));
    return len;
}

void RouterOutput::set_sample_rate(int rate) {
    sample_rate = rate;
    router->set_bt_sample_rate(rate);

    ESP_LOGI(OSP_TAG, "BT sample rate: %d", rate);
}

void RouterOutput::set_output_active(bool active) {
    active = active;
    router->set_bt_active(active);

    ESP_LOGI(OSP_TAG, "BT output active: %d", active);
}
