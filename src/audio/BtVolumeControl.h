#pragma once

#include "BluetoothA2DPSink.h"
#include "state/AppManager.h"
#include "helpers.h"


class BtVolumeControl : public A2DPVolumeControl {
public:
    BtVolumeControl();

    virtual void update_audio_data(Frame* data, uint16_t frameCount);

    virtual void set_volume(uint8_t volume) {
        volumeFactor = volume;
        ESP_LOGE("BT_CONTROL", "volume set: %d", volume);
    }

    void set_muted(bool muted) {
        this->muted = muted;
    }

protected:
    volatile bool muted = false;
    Mult volume_mult;
};
