#include "AudioProcessor.h"
#include "math.h"
#include "helpers.h"
#include "config.h"
#include "ble/BLEManager.h"

using ShapeFunction = float (*)(float);
ShapeFunction shape_functions[] = {sin_fast, saw, tri, square};


// ==================================
// ======== AUDIO PROCESSOR =========
// ==================================
AudioProcessor::AudioProcessor(AppManager* manager) {
    this->manager = manager;
    volumeFactorMax = 128;
    volumeFactor = 128;
    is_volume_used = true;
}


void AudioProcessor::update_audio_data(Frame* data, uint16_t frameCount) {
    if (data == nullptr || frameCount <= 0) return;

    /* =====================
    *  => SETUP LOOP
    *  ===================== */
    const float SR = 44100.0f;
    int32_t pcm_swap;
    State state = manager->state;

    // simply zero the buffer and return if we are not using bt
    if(state.source_mode != SOURCE_BT) {
        memset(data, 0, sizeof(Frame) * frameCount);
        return;
    };

    // >>> MULTS
    // ------------------------
    const float volume_mult = fclamp((float)volumeFactor / volumeFactorMax, 0.f, 1.f);
    const float l_mult_target = fclamp((float)state.l_factor / LR_FACTOR_MAX, 0.f, 1.f) * volume_mult;
    const float r_mult_target = fclamp((float)state.r_factor / LR_FACTOR_MAX, 0.f, 1.f) * volume_mult;
    const float lr_mult_step = 1.f / LR_RISE_TIME / SR;

    const bool is_vibe_active = state.ble_connected;
    const float vibe_step = 1.f / VIBE_RISE_TIME / SR;
    const float vibe_target = is_vibe_active ? BLEManager::vibe_level_to_mult(state.vibe_level) : 1.0f;

    // const float duck_step = 1.f / DUCK_RISE_TIME / SR;
    // const float duck_target = duck_level <= 0 ? 1.f : (duck_level == 1 ? DUCK_LEVEL_1 : DUCK_LEVEL_2);

    const bool requires_intensity = state.bt_mode == BTMODE_AM || state.bt_mode == BTMODE_TENS;
    const ShapeFunction shape_function = shape_functions[state.bt_mode_am_shape];
    const int tens_period_samples = roundf(SR / state.bt_mode_tens_freq);
    const int tens_pulse_samples = state.bt_mode_tens_width;

    if(!requires_intensity) {
        intensity_l = 0.f;
        intensity_r = 0.f;
    }

    /* =====================
    *  => LOOP
    *  ===================== */
    // long start = millis();
    for (int i=0; i<frameCount; i++){
        int32_t pcm_l = data[i].channel1;
        int32_t pcm_r = data[i].channel2;
        
        // >>> BT MODE: am, tens
        // ------------------------
        if(requires_intensity) {
            // calculate intensity for LR channels
            const float norm_abs_l = (float)abs(pcm_l) / (float)INT16_MAX * SQRT2;
            const float norm_abs_r = (float)abs(pcm_r) / (float)INT16_MAX * SQRT2;
            intensity_l = intensity_alpha * norm_abs_l + (1.f - intensity_alpha) * intensity_l;
            intensity_r = intensity_alpha * norm_abs_r + (1.f - intensity_alpha) * intensity_r;

            // calculate new wave (either AM or TENS) between -1.0..1.0
            float wave_l = 0.f, wave_r = 0.f;

            if(state.bt_mode == BTMODE_AM) {
                float t = (float)time / SR * state.bt_mode_am_freq;
                wave_l = shape_function(t);
                wave_r = shape_function(t);
            }
            else if(state.bt_mode == BTMODE_TENS) {
                wave_l = tens(time,                             tens_period_samples, tens_pulse_samples);
                wave_r = tens((time + tens_period_samples/2),   tens_period_samples, tens_pulse_samples);
            }
            
            // write new wave into PCM
            pcm_l = (INT16_MAX - 1) * fclamp(wave_l * intensity_l, -1.f, 1.f);
            pcm_r = (INT16_MAX - 1) * fclamp(wave_r * intensity_r, -1.f, 1.f);
        }

        // >>> BT MODE: pulse
        // ------------------------
        float pulse_mult = 1.0f;

        if(state.bt_mode == BTMODE_PULSE) {
            const float t = (float)time / SR * state.bt_mode_pulse_freq;
            // convert from 0..100 to -1..1
            const float bias = state.bt_mode_pulse_bias / 100.f * 2.f - 1.f;
            // compute biased am modulation
            const float pm = (sin_fast(t) + bias) / (1.f + bias) * .997f;
            pulse_mult = fclamp(pm, 0.f, 1.f);
        }

        // >>> SETTINGS
        // ------------------------
        if(state.settings_invert_phase) {
            pcm_r = -pcm_r;
        }

        if(state.settings_invert_lr) {
            pcm_swap = pcm_l;
            pcm_l = pcm_r;
            pcm_r = pcm_swap;
        }

        // >>> VOLUME
        // ------------------------
        l_mult.smooth_increase(l_mult_target, lr_mult_step);
        r_mult.smooth_increase(r_mult_target, lr_mult_step);

        vibe_mult.smooth_update(vibe_target, vibe_step);
        const float lr_mult = pulse_mult * vibe_mult.value;

        pcm_l = pcm_l * (l_mult.value * lr_mult);
        pcm_r = pcm_r * (r_mult.value * lr_mult);

        // >>> WRITE RESULTS
        // ------------------------
        data[i].channel1 = clamp(pcm_l, INT16_MIN, INT16_MAX);
        data[i].channel2 = clamp(pcm_r, INT16_MIN, INT16_MAX);
        time += 1;
    }

    // Serial.print(l_mult.value * 100);
    // Serial.print(", ");
    // Serial.print(r_mult.value * 100);
    // Serial.print(", ");
    // Serial.println(vibe_mult.value * 100);
}
