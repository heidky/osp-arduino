#include "CommonProcessing.h"
#include "config.h"
#include "ble/BLEManager.h"

CommonProcessing::CommonProcessing()
{
    vibe_mult.value = 1.f;
    remote_mult.value = 1.f;
}

void CommonProcessing::process(State &state, Frame *data, size_t frame_count, int32_t SR)
{
    const float l_mult_target = fclamp((float)state.l_factor / LR_FACTOR_MAX, 0.f, 1.f);
    const float r_mult_target = fclamp((float)state.r_factor / LR_FACTOR_MAX, 0.f, 1.f);
    const float lr_mult_step = 1.f / LR_RISETIME / SR;

    const bool is_vibe_active = state.vibe_active;
    const float vibe_step = 1.f / VIBE_RISETIME / SR;
    const float vibe_target = is_vibe_active ? BLEManager::vibe_level_to_mult(state.vibe_level) : 1.0f;

    const float remote_target = state.remote_score == 0 ? 1.f : (state.remote_score > 0 ? REMOTE_BOOST_MULT : REMOTE_DUCK_MULT);
    const float remote_duck_step = 1.f / REMOTE_DUCK_RISETIME / SR;
    const float remote_boost_step = 1.f / (state.remote_score > 0 ? REMOTE_BOOST_RISETIME : REMOTE_BOOST_FALLTIME) / SR;

    int32_t pcm_swap = 0;

    for(size_t i = 0; i < frame_count; i++) 
    {
        int32_t pcm_l = data[i].channel1;
        int32_t pcm_r = data[i].channel2;

        // UPDATE METER (once per bt data batch)
        // ------------------------
        if(time % (1024/4) == 0) {
            const float norm_abs_l = (float)abs(pcm_l) / INT16_MAX * SQRT2;
            const float norm_abs_r = (float)abs(pcm_r) / INT16_MAX * SQRT2;
            meter_l.smooth_update(norm_abs_l);
            meter_r.smooth_update(norm_abs_r);
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

        if(state.remote_score < 0 || remote_mult.value <= 0.99f)
            remote_mult.smooth_increase(remote_target, remote_duck_step);
        else
            remote_mult.linear_update(remote_target, remote_boost_step, REMOTE_BOOST_MULT);

        const float lr_mult = vibe_mult.value * remote_mult.value;

        pcm_l = pcm_l * (l_mult.value * lr_mult);
        pcm_r = pcm_r * (r_mult.value * lr_mult);

        // >>> WRITE RESULTS
        // ------------------------
#ifdef LR_HARDWARE_INVERTED
        data[i].channel1 = clamp(pcm_r, INT16_MIN, INT16_MAX);
        data[i].channel2 = clamp(pcm_l, INT16_MIN, INT16_MAX);
#else
        data[i].channel1 = clamp(pcm_l, INT16_MIN, INT16_MAX);
        data[i].channel2 = clamp(pcm_r, INT16_MIN, INT16_MAX);
#endif
        time += 1;
    }
}

void CommonProcessing::reset()
{
}
