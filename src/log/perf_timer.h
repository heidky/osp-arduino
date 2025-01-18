#include <esp_timer.h>
#include <Arduino.h>


#define START_PERF(n) \
static uint32_t __count_##n = 0; \
static uint32_t __accum_##n = 0; \
uint32_t __start_##n = esp_timer_get_time();


#define STOP_PERF(n, every) \
__count_##n ++; \
uint32_t __diff_##n = esp_timer_get_time() - __start_##n; \
__accum_##n = __accum_##n > __diff_##n ? __accum_##n : __diff_##n; \
if(__count_##n % every == 0) { Serial.print("[" #n "]> us:"); Serial.println(__accum_##n);  __accum_##n = 0; }  //TODO: dont use Serial
