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

    ExpAvg meter_l = ExpAvg(0.01f);
    ExpAvg meter_r = ExpAvg(0.01f);
protected:
    volatile bool muted = false;
    Mult volume_mult;
};
