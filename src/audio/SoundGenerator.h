#pragma once
#include "state/AppManager.h"
#include <A2DPVolumeControl.h>


class SoundGenerator {
    friend void task_handler(void *arg);

public:
    SoundGenerator(AppManager *manager);
    void start();


    Frame *receive_chunk(size_t frame_count);

protected:
    AppManager *manager;
    TaskHandle_t task_handle;
    uint32_t chunk_samples = 128;
    float sample_rate = 44100.f;

    uint32_t time = 0;
    bool is_buffer_cleared;

    void task_code();
    void generate_chunk(Frame *data, uint32_t frame_count);
    void clear_buffer();
};