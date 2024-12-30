#include "helpers.h"
#include "math.h"

// fast sin implementation, abs error: 0.0029
float sin_fast(float x) {
    float tmp;
    const float p = modf(x, &tmp) * PI_2;
    const float p1 = p <= PI_1 ? p-1.570796f : p-4.712388f;
    const float r2 = p1 * p1;
    const float res = 1.f - r2/2.f + r2*r2/26.05072f;
    return p <= PI_1 ? res : -res;
}

float fclamp(float x, float min, float max) {
    return x < min ? min : (x > max ? max : x);
}

int32_t clamp(int32_t x, int32_t min, int32_t max) {
    return x < min ? min : (x > max ? max : x);
}

float saw(float x) {
    float tmp;
    return modf(x, &tmp) * 2.f - 1.f;
}

float tri(float x) {
    float tmp;
    float res = modf(x, &tmp);
    res = res <= 0.5f ? res : (1.f - res);
    return res * 4.f - 1.f;
}

float square(float x) {
    float tmp;
    float res = modf(x, &tmp);
    return res <= 0.5f ? 1.f : -1.f;
}


// ==================================
// ======== MULT            =========
// ==================================
Mult::Mult() {
    value = 0.0;
}

void Mult::smooth_update(float target, float step) {
    const float delta = target - value;
    float new_value = value;

    if(abs(delta) < step * 1.01)  new_value = target;
    else if (delta > 0) new_value += step;
    else if (delta < 0) new_value -= step;

    value = fclamp(new_value, 0.0f, 1.0f);
}

void Mult::smooth_increase(float target, float step) {
    const float delta = target - value;
    float new_value = value;

    if(abs(delta) < step * 1.01)  new_value = target;
    else if (delta > 0) new_value += step;
    else if (delta < 0) new_value = target;

    value = fclamp(new_value, 0.0f, 1.0f);
}
