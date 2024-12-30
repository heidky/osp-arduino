#pragma once

#include "BluetoothA2DPSink.h"
#include "state/AppManager.h"
#include "helpers.h"


class AudioProcessor : public A2DPVolumeControl {
public:
    AppManager *manager;

    uint32_t time = 0;

    float intensity_l = 0;
    float intensity_r = 0;
    float intensity_alpha = 0.001;

    // float duck_current = 0.0;
    Mult l_mult;
    Mult r_mult;
    Mult vibe_mult;

    AudioProcessor(AppManager* manager);

    virtual void update_audio_data(Frame* data, uint16_t frameCount);

    virtual void set_volume(uint8_t volume) {
        volumeFactor = volume;
        // Serial.print("volume set: ");
        // Serial.println(volume);
    }
};
