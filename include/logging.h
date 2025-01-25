#pragma once

#include <Arduino.h>
#include <esp_log.h>

// ========== TAGS =======
extern const char *OSP_TAG;
extern const char *I2SR_TAG;
extern const char *SOUND_GEN_TAG;
extern const char *APPM_TAG;

inline void log_init() {
    // none of this works due to the arduino framework being dumb
    Serial.begin(115200);
#ifdef OSP_LOG_LEVEL
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(OSP_TAG, (esp_log_level_t)OSP_LOG_LEVEL);
    esp_log_level_set(I2SR_TAG, (esp_log_level_t)OSP_LOG_LEVEL);
    esp_log_level_set(SOUND_GEN_TAG, (esp_log_level_t)OSP_LOG_LEVEL);
    esp_log_level_set(APPM_TAG, (esp_log_level_t)OSP_LOG_LEVEL);
#endif    
}
