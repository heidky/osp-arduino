#include "SoundGenerator.h"
#include <Arduino.h>
#include "helpers.h"

#define SOUND_GEN_TAG "SOUND_GEN"

#define SOUND_GEN_BUFF_SAMPLES (128)
Frame buffer[SOUND_GEN_BUFF_SAMPLES*2];

void task_handler(void *arg) {   
    SoundGenerator *generator = (SoundGenerator*)arg;
    generator->task_code();
}

SoundGenerator::SoundGenerator(AppManager *manager) : manager(manager) {
    chunk_samples = SOUND_GEN_BUFF_SAMPLES;
}

void SoundGenerator::start() {
    // const BaseType_t priority = 3;
    // const BaseType_t core = 1;
    // const uint32_t stack = 2048;

    // BaseType_t result = xTaskCreatePinnedToCore(task_handler, "SoundGenTask", stack, (void*)this, priority, &task_handle, core);
    // if (result != pdPASS)   ESP_LOGE(SOUND_GEN_TAG, "xTaskCreatePinnedToCore");
    // else                    ESP_LOGI(SOUND_GEN_TAG, "SoundGenerator Started");
}

Frame *SoundGenerator::receive_chunk(size_t frame_count)
{
    if (frame_count > SOUND_GEN_BUFF_SAMPLES) {
        ESP_LOGE(SOUND_GEN_TAG, "xTaskCreatePinnedToCore");
        return NULL;
    }

    generate_chunk(buffer, frame_count);
    return buffer;
}

Frame *SoundGenerator::receive_chunk(size_t *frame_count)
{
    generate_chunk(buffer, chunk_samples);
    *frame_count = chunk_samples;
    return buffer;
}

void SoundGenerator::task_code() {
    for (;;) {
        // State state = manager->state;
        
        // if(state.source_mode == SOURCE_GEN) {
        //     generate_chunk(buffer, chunk_samples);
        //     router->write_data_gen(buffer, chunk_samples);
        // } 
        // else {
        //     if(!is_buffer_cleared) clear_buffer();
        // }

        delay(5);
    }
}



void SoundGenerator::generate_chunk(Frame *data, uint32_t frame_count)
{   
    for(uint32_t i = 0; i < frame_count; i++) {
        const float t = (float)time / sample_rate;
        data[i].channel1 = (0.4 * sin_fast(1000.f * t)) * (INT16_MAX - 1);
        data[i].channel2 = (0.4 * sin_fast(1000.f * t)) * (INT16_MAX - 1);
        time++;
    }

    is_buffer_cleared = false;
}

void SoundGenerator::clear_buffer() {
    memset(buffer, 0, sizeof(Frame) * SOUND_GEN_BUFF_SAMPLES);
    is_buffer_cleared = true;
}
