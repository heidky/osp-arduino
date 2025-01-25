#include "BTOutput.h"
#include <Arduino.h>
#include "logging.h"


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

    ESP_LOGI(OSP_TAG, "BT sample rate: %d", rate);
}

void BtOutput::set_output_active(bool active) {
    active = active;
    router->set_bt_active(active);

    ESP_LOGI(OSP_TAG, "BT output active: %d", active);
}
