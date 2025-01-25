#include "SoundGenerator.h"
#include <Arduino.h>
#include "helpers.h"
#include "logging.h"


SoundGenerator::SoundGenerator(AppManager *manager) : manager(manager) {}

bool SoundGenerator::begin()
{
    if(buffer != NULL) {
        ESP_LOGE(SOUND_GEN_TAG, "double SoundGenerator begin()");
        return false;
    }

    buffer = (Frame*)calloc(buffer_frame_count, sizeof(Frame));

    if(buffer == NULL) {
        ESP_LOGE(SOUND_GEN_TAG, "cannot initialize SoundGenerator buffer. out of memory (likely)");
        return false;
    }

    return true;
}

void SoundGenerator::end()
{
    if(buffer != NULL) {
        free(buffer);
    }
    else {
        ESP_LOGE(SOUND_GEN_TAG, "SoundGenerator cannot end(). buffer is NULL");
    }
}

Frame *SoundGenerator::receive_chunk(size_t frame_count)
{
    if (frame_count > buffer_frame_count) {
        ESP_LOGE(SOUND_GEN_TAG, "chunk is too big. cannot generate");
        return NULL;
    }

    generate_chunk(buffer, frame_count);
    return buffer;
}

void SoundGenerator::generate_chunk(Frame *data, uint32_t frame_count)
{   
    static const float TRACK_NORM = 0.707f; // -3db (fs)

    const float freq = 440.f;

    for(uint32_t i = 0; i < frame_count; i++) {
        const float t = (float)time / sample_rate;
        data[i].channel1 = (TRACK_NORM * sin_fast(freq * t)) * (INT16_MAX - 1);
        data[i].channel2 = (TRACK_NORM * sin_fast(freq * t)) * (INT16_MAX - 1);
        time++;
    }

    is_buffer_cleared = false;
}

void SoundGenerator::clear_buffer() {
    memset(buffer, 0, sizeof(Frame) * buffer_frame_count);
    is_buffer_cleared = true;
}
