#pragma once 
#include <Arduino.h>

#define PI_2    6.283185f
#define PI_1    3.141592f
#define SQRT2   1.414213f

int32_t clamp(int32_t x, int32_t min, int32_t max);
// fast sin implementation, abs error: 0.0029
float fclamp(float x, float min, float max);

float sin_fast(float x);
float saw(float x);
float tri(float x);
float square(float x);

inline float tens(uint32_t time, int period_samples, int high_samples) {
    const int t_period = time % period_samples;
    const int t_perdid_m180 = t_period - period_samples/2;

    if(t_period <= high_samples) {
        return 1.f;
    }
    else 
    if (t_perdid_m180 >= 0 && t_perdid_m180 <= high_samples) {
        return -1.f;
    }

    return 0.f;
}


struct Mult {
    float value = 0.f;

    Mult();
    void smooth_update(float target, float step);
    void smooth_increase(float target, float step);
    void linear_update(float target, float step, float max = 1.f);
};

struct ExpAvg {
    float value = 0.f;
    float alpha;

    ExpAvg(float alpha): alpha(alpha) {}
    inline void smooth_update(float new_value) {
        this->value = new_value * alpha + this->value * (1.f-alpha); 
    }
};
