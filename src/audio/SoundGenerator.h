#pragma once
#include "state/AppManager.h"
#include <A2DPVolumeControl.h>


struct SoundGeneratorConfig {
    float frequency = 400; // [200-1000]
    int32_t shape = 0; 
    int32_t lr_delta = 0; // < 0 is phase [0-180], >= 0 frequency delta*50 [0, 100] 
    // float lr_phase_offset = 0;

    int32_t pulse_shape = 0;
    int32_t pulse_freq = 3; // [1-10]
    int32_t pulse_bias = 3; // [1-10]

    int32_t am_shape = 0;
    float am_freqency = 0;
    float am_depth = 0;
};

class SoundGenerator {
public:
    SoundGenerator(AppManager *manager);

    bool begin();
    void end();

    Frame *receive_chunk(size_t frame_count);

    inline int32_t get_sample_rate() { return sample_rate; }

protected:
    Frame *buffer = NULL;
    size_t buffer_frame_count = 512;

    AppManager *manager;
    TaskHandle_t task_handle;
    int32_t sample_rate = 44100;

    uint32_t time = 0;
    bool is_buffer_cleared;

    void generate_chunk(Frame *data, uint32_t frame_count);
    void clear_buffer();

    float phase_l = 0;
    float phase_r = 0;

    float phase_pulse = 0;
    float phase_am = 0;
};