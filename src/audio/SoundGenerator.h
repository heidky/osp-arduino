#pragma once
#include "state/AppManager.h"
#include <A2DPVolumeControl.h>


class SoundGenerator {
public:
    SoundGenerator(AppManager *manager);

    bool begin();
    void end();

    Frame *receive_chunk(size_t frame_count);

protected:
    Frame *buffer = NULL;
    size_t buffer_frame_count = 512;

    AppManager *manager;
    TaskHandle_t task_handle;
    float sample_rate = 44100.f;

    uint32_t time = 0;
    bool is_buffer_cleared;

    void generate_chunk(Frame *data, uint32_t frame_count);
    void clear_buffer();
};