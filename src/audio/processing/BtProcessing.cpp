#include "BtProcessing.h"

using ShapeFunction = float (*)(float);
ShapeFunction shape_functions[] = {sin_fast, saw, tri, square};

BtProcessing::BtProcessing() {}

void BtProcessing::process(State &state, Frame *data, size_t frame_count, int32_t SR)
{
    const bool requires_intensity = state.bt_mode == BTMODE_AM || state.bt_mode == BTMODE_TENS;
    const ShapeFunction shape_function = shape_functions[state.bt_mode_am_shape];
    const int tens_period_samples = roundf(SR / state.bt_mode_tens_freq);
    const int tens_pulse_samples = state.bt_mode_tens_width;

    if(!requires_intensity) {
        intensity_l.value = 0.f;
        intensity_r.value = 0.f;
    }

    for(size_t i = 0; i < frame_count; i++) 
    {
        int32_t pcm_l = data[i].channel1;
        int32_t pcm_r = data[i].channel2;

         // >>> BT MODE: am, tens
        // ------------------------
        if(requires_intensity) {
            // calculate intensity for LR channels
            const float norm_abs_l = (float)abs(pcm_l) / (float)INT16_MAX * SQRT2;
            const float norm_abs_r = (float)abs(pcm_r) / (float)INT16_MAX * SQRT2;
            intensity_l.smooth_update(norm_abs_l);
            intensity_r.smooth_update(norm_abs_r);

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
            pcm_l = (INT16_MAX - 1) * fclamp(wave_l * intensity_l.value, -1.f, 1.f);
            pcm_r = (INT16_MAX - 1) * fclamp(wave_r * intensity_r.value, -1.f, 1.f);
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

        // >>> WRITE RESULTS
        // ------------------------
        data[i].channel1 = clamp(pcm_l * pulse_mult, INT16_MIN, INT16_MAX);
        data[i].channel2 = clamp(pcm_r * pulse_mult, INT16_MIN, INT16_MAX);
        time += 1;
    }
}

void BtProcessing::reset()
{
    intensity_l.value = 0.f;
    intensity_r.value = 0.f;
}
