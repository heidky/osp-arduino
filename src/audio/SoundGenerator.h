#pragma once
#include "state/AppManager.h"
#include <A2DPVolumeControl.h>



class SoundGenerator {
public:
    SoundGenerator(AppManager *manager);

    bool begin();
    void end();

    Frame *receive_chunk(size_t frame_count, const SoundGeneratorConfig &config);

    inline int32_t get_sample_rate() { return sample_rate; }

protected:
    SoundGeneratorConfig config;

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