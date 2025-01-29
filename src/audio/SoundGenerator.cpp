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

Frame *SoundGenerator::receive_chunk(size_t frame_count, const SoundGeneratorConfig &config)
{
    if (frame_count > buffer_frame_count) {
        ESP_LOGE(SOUND_GEN_TAG, "chunk is too big. cannot generate");
        return NULL;
    }

    this->config = config;
    generate_chunk(buffer, frame_count);
    return buffer;
}

// float phase_norm(float x) {
//     return x >= 0 ? x : x + 1.0;
// }

// float phase_diff(float src, float dst) {
//     const float diff = fmodf(dst - src, 1.f);
//     return diff > 0.5 ? diff - 1.f : (diff < -0.5f ? diff + 1.f : diff);  // bound between -.5 to +.5
// }

void SoundGenerator::generate_chunk(Frame *data, uint32_t frame_count)
{   
    // static const float TRACK_NORM = 0.70794578f;    // -3db (fs)
    static const float AUDIO_NORMALIZATION = 0.50118723f;    // -6db (fs)
    static const int16_t FS = (INT16_MAX - 1) * AUDIO_NORMALIZATION;

    for(uint32_t i = 0; i < frame_count; i++) {
        const float phase_delta_l = (float)config.frequency / sample_rate;
        float phase_delta_r;

        if(config.lr_delta > 0) {
            phase_delta_r = phase_delta_l + ((float)config.lr_delta / sample_rate);
        }
        else {
            // const float current_phase_offset = phase_l - phase_r;
            // const float desired_phase_offset = (float)abs(config.lr_delta)/360.f;

            // const float diff = phase_diff(current_phase_offset, desired_phase_offset);
            // const float phase_align_step = min(abs(diff), 0.5f / sample_rate * 2);
            // phase_delta_r = phase_delta_l + (diff > 0 ? -phase_align_step : phase_align_step);
    
            phase_delta_r = phase_delta_l;
            phase_r = phase_l + (-config.lr_delta)/360.f;
        }

        phase_l = fmodf(phase_l + phase_delta_l, 1.f);
        phase_r = fmodf(phase_r + phase_delta_r, 1.f);

        const float l = sin_fast(phase_l);
        const float r = sin_fast(phase_r);

        float mult_pulse = 1.f;

        if(config.pulse_shape > 0) {
            const float phase_delta_pulse = (float)config.pulse_freq / sample_rate;
            phase_pulse = fmodf(phase_pulse + phase_delta_pulse, 1.f);
            
            const float bias = (float)config.pulse_bias / 100.f * 2.f - 1.f; // convert from 0..100 to -1..1
            const float pm = (sin_fast(phase_pulse) + bias) / (1.f + bias) * .997f; // compute biased am modulation
            mult_pulse = fclamp(pm, 0.f, 1.f);
        }

        float mult_am = 1.f;

        if(config.am_shape > 0) {
            const float phase_delta_am = (float)config.am_freqency / sample_rate;
            phase_am = fmodf(phase_am + phase_delta_am, 1.f);
            mult_am = (sin_fast(phase_am) / 2.f + 0.5f) * config.am_depth + (1.f - config.am_depth);
        }

        const float mult = fclamp(mult_pulse * mult_am, 0.f, 1.f);

        data[i].channel1 = l * mult * FS;
        data[i].channel2 = r * mult * FS;
        time++;
    }

    is_buffer_cleared = false;
}

void SoundGenerator::clear_buffer() {
    memset(buffer, 0, sizeof(Frame) * buffer_frame_count);
    is_buffer_cleared = true;
}
