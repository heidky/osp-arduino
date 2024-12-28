#pragma once

#include "BluetoothA2DPSink.h"
#include "state/AppManager.h"

#define LR_FACTOR_MAX 99


class AudioProcessor : public A2DPVolumeControl {
public:
    AppManager *manager;

    int32_t l_factor;
    int32_t r_factor;

    uint32_t time = 0;

    float l_intensity = 0;
    float r_intensity = 0;
    float intensity_alpha = 0.001;

    float duck_current = 0.0;
    float vibe_mult = 0.0;

    AudioProcessor(AppManager* manager);

    virtual void update_audio_data(Frame* data, uint16_t frameCount);

    virtual void set_volume(uint8_t volume) {
        Serial.print("volume set: ");
        Serial.println(volume);
        
        volumeFactor = volume;
    }

    virtual void set_l_volume(int32_t volume) {
        if (volume > LR_FACTOR_MAX) {
            volume = LR_FACTOR_MAX;
        }
        else if(volume < 0) {
            volume = 0;
        }

        l_factor = volume;
    }

    virtual void set_r_volume(int32_t volume) {
        if (volume > LR_FACTOR_MAX) {
            volume = LR_FACTOR_MAX;
        }
        else if(volume < 0) {
            volume = 0;
        }

        r_factor = volume;
    }

    // void set_enabled(bool enabled) override {
    //     // do not allow to disable this
    //     is_volume_used = true;
    // }
};