#include "AudioProcessor.h"
#include "math.h"


#define PI_2    6.283185f
#define PI_1    3.141592f

// fast sin implementation, abs error: 0.0029
static float fsin(float x) {
    float tmp;
    const float p = modf(x, &tmp) * PI_2;
    const float p1 = p <= PI_1 ? p-1.570796f : p-4.712388f;
    const float r2 = p1 * p1;
    const float res = 1.f - r2/2.f + r2*r2/26.05072f;
    return p <= PI_1 ? res : -res;
}

static float saw(float x) {
    float tmp;
    return modf(x, &tmp) * 2.f - 1.f;
}

static float tri(float x) {
    float tmp;
    float res = modf(x, &tmp);
    res = res <= 0.5f ? res : (1.f - res);
    return res * 4.f - 1.f;
}

static float square(float x) {
    float tmp;
    float res = modf(x, &tmp);
    return res <= 0.5f ? 1.f : -1.f;
}

using ShapeFunction = float (*)(float);
ShapeFunction shape_functions[] = {fsin, saw, tri, square};


AudioProcessor::AudioProcessor(AppManager* manager) {
    this->manager = manager;
    volumeFactorMax = 128;
    volumeFactor = 128;
    is_volume_used = true;
}


void AudioProcessor::update_audio_data(Frame* data, uint16_t frameCount) {
    if (data != nullptr && frameCount > 0) {
        // read state
        int16_t l_factor = manager->state.l_factor;
        int16_t r_factor = manager->state.r_factor;
        float tmp;

        int mode = manager->state.bt_mode;
        float freq = manager->state.bt_mode_am_freq;
        ShapeFunction shape_function = shape_functions[manager->state.bt_mode_am_shape];

        const bool invert_phase = manager->state.settings_invert_phase;
        const bool invert_lr = manager->state.settings_invert_lr;

        const int bt_mode = manager->state.bt_mode;
        const bool requires_intensity = bt_mode == BTMODE_AM || bt_mode == BTMODE_TENS;

        const int tens_period_samples = roundf(44100.f / manager->state.bt_mode_tens_freq);
        const int tens_pulse_samples = manager->state.bt_mode_tens_width;

        float pulse_freq = manager->state.bt_mode_pulse_freq;
        float pulse_bias = manager->state.bt_mode_pulse_bias;

        const float volume_mult = (float)volumeFactor / volumeFactorMax;

        // reset intensity if bypass
        if(!requires_intensity) {
            l_intensity = 0;
            r_intensity = 0;
        }

        const float l_mult = ((float)l_factor / LR_FACTOR_MAX) * volume_mult;
        const float r_mult = ((float)r_factor / LR_FACTOR_MAX) * volume_mult;

        // long start = millis();

        for (int i=0; i<frameCount; i++){
            int32_t l_pcm = data[i].channel1;
            int32_t r_pcm = data[i].channel2;

            // bt mode: am, tens
            if(requires_intensity) {
                l_intensity = intensity_alpha * abs(l_pcm) + (1.f - intensity_alpha) * l_intensity;
                r_intensity = intensity_alpha * abs(r_pcm) + (1.f - intensity_alpha) * r_intensity;
                float l = 0.f, r = 0.f;

                if(bt_mode == BTMODE_AM) {
                    float t = time / 44100.f * freq;
                    l = shape_function(t);
                    r = shape_function(t);
                }
                else if(bt_mode == BTMODE_TENS) {
                    int t = time % tens_period_samples;

                    if(t < tens_pulse_samples) {
                        l = r = 1.f;
                    }else if (t >= tens_period_samples/2 && t < tens_period_samples/2 + tens_pulse_samples) {
                        l = r = -1.f;
                    }
                }
                
                l_pcm = l * l_intensity * 1.5f;
                r_pcm = r * r_intensity * 1.5f;
            }

            if(bt_mode == BTMODE_PULSE) {
                float t = time / 44100.f * pulse_freq;
                // convert from 0..100 to -1..1
                const float b = pulse_bias / 100.f * 2 - 1;
                // compute biased am modulation
                float mult = (fsin(t) + b) / (1 + b) * .997;
                if(mult < 0) mult = 0;

                l_pcm = l_pcm * mult;
                r_pcm = r_pcm * mult;
            }

            // invert
            if(invert_phase) {
                r_pcm *= -1;
            }

            if(invert_lr) {
                int32_t tmp = l_pcm;
                l_pcm = r_pcm;
                r_pcm = tmp;
            }

            // volume
            l_pcm = l_pcm * l_mult;
            r_pcm = r_pcm * r_mult;

            // write result
            data[i].channel1 = l_pcm;
            data[i].channel2 = r_pcm;
            time += 1;
        }

        // Serial.println(millis()-start);
    }
}




// adjust the volume
// if mono -> we provide the same output on both channels
// if (mono_downmix) {
//     r_pcm = pcmLeft = (pcmLeft + r_pcm) / 2;
// }

// uint16_t start = i - AM_WINDOW_RADIUS;
// if(start < 0) start = 0;

// uint16_t end = i + AM_WINDOW_RADIUS + 1;
// if(end > frameCount) end = frameCount;

// int16_t count = end - start;
// float sum_l = 0.f, sum_r = 0.f;

// for(int j = start; j < end; j++) {
//     sum += 

// }
// float t = 2 * PI * ((float)time / 44100.f) * freq;
// pcmLeft = sin(t) * 32760;
// pcmLeft = sin(t) * 32760;

// switch(shape) {
// case SHAPE_SIN: {
//     l = fsinf(t);
//     r = fsinf(t);
//     break;
// }
// case SHAPE_SAW: {
//     l = saw(t);
//     r = saw(t);
//     break;
// }
// case SHAPE_TRI: {
//     l = tri(t);
//     r = tri(t);
//     break;
// }
// case SHAPE_SQUARE: {
//     l = square(t);
//     r = square(t);             
//     break;
// }
// }