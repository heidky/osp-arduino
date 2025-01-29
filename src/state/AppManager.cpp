#include "AppManager.h"
#include "logging.h"


AppManager::AppManager() {
    state_mutex = xSemaphoreCreateMutex();
}

bool AppManager::copy_state_into(State &out_state) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        out_state = this->state;
        xSemaphoreGive(state_mutex);
        return true;
    }
    else {
        ESP_LOGE(APPM_TAG, "cannot copy app state safely");
        return false;
    }
}

void AppManager::action(Action a, void* payload) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(APPM_TAG, "cannot perform action due state_mutex take failure.");
        return;
    };

    switch (state.page)
    {
    case Page::Main:
        page_Main(a, payload);
        break;
    case Page::BtMode_Selection:
        page_BtMode_Selection(a, payload);
        break;
    case Page::BtMode_Pulse:
        page_BtMode_Pulse(a, payload);
        break;
    case Page::BtMode_AM:
        page_BtMode_AM(a, payload);
        break;
    case Page::BtMode_Tens:
        page_BtMode_Tens(a, payload);
        break;

    case Page::Settings:
        page_Settings(a, payload);
        break;
    case Page::Settings_Source:
        page_Settings_Source(a, payload);
        break;
    case Page::Settings_Invert:
        page_Settings_Invert(a, payload);
        break;
    case Page::Settings_Ramp:
        page_Settings_Ramp(a, payload);
        break;

    case Page::Generator:
        page_Generator(a, payload);
        break;
    }

    xSemaphoreGive(state_mutex);
}


void AppManager::page_Main(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        set_l_factor(state.l_factor + d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        set_r_factor(state.r_factor + d);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        if(state.source_mode == SOURCE_BT) {
            set_page(Page::BtMode_Selection);
        }
        else if(state.source_mode == SOURCE_GEN) {
            set_page(Page::Generator);
        }
        break;
    }
    case Action::DoublePressM: {
        set_page(Page::Settings);
        break;
    }
    }
}

void AppManager::page_BtMode_Selection(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL:
    case Action::ValueR: {
        int d = *(int*)payload;
        inc_bt_mode(d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        state.bt_mode = state.bt_mode_cursor;

        if(state.bt_mode == BTMODE_PULSE) {
            set_page(Page::BtMode_Pulse);
        }
        else if(state.bt_mode == BTMODE_AM) {
            set_page(Page::BtMode_AM);
        }
        else if(state.bt_mode == BTMODE_TENS) {
            set_page(Page::BtMode_Tens);
        }
        else {
            set_page(Page::Main);
        }

        break;
    }
    case Action::DoublePressM: {
        set_page(Page::Settings);
        break;
    }
    }
}

void AppManager::page_BtMode_Pulse(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        int f = state.bt_mode_pulse_freq;
        set_pulse_freq(f + PULSE_FREQ_STEP * d);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        int b = state.bt_mode_pulse_bias;
        set_pulse_bias(b + PULSE_BIAS_STEP * d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
        break;
    }
    }
}

void AppManager::page_BtMode_AM(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        inc_bt_am_shape(d);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        int f = state.bt_mode_am_freq;
        set_bt_am_freq(f + FREQ_STEP * d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
        break;
    }
    }
}

void AppManager::page_BtMode_Tens(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        int w = state.bt_mode_tens_width;
        set_tens_width(w + TENS_WIDTH_STEP * d);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        int f = state.bt_mode_tens_freq;
        set_tens_freq(f + TENS_FREQ_STEP * d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
        break;
    }
    }
}

void AppManager::page_Settings(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL:
    case Action::ValueR: {
        int d = *(int*)payload;
        inc_settings_cursor(d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        const int s = state.settings_cursor;

        if(s == SETTINGS_SOURCE) {
            set_page(Page::Settings_Source);
        }
        else if(s == SETTINGS_INVERT) {
            set_page(Page::Settings_Invert);
        }
        else if(s == SETTINGS_RAMP) {
            set_page(Page::Settings_Ramp);
        }
        break;
    }
    }
}

void AppManager::page_Settings_Source(Action a, void *payload)
{
    switch (a) 
    {
    case Action::ValueL:
    case Action::ValueR: {
        int d = *(int*)payload;
        inc_source_cursor(d);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_source_mode(state.source_mode_cursor);
        set_page(Page::Main);
        break;
    }
    }
}

void AppManager::page_Settings_Invert(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        set_invert_lr(!state.settings_invert_lr);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        set_invert_phase(!state.settings_invert_phase);
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
    }
    }
}

void AppManager::page_Settings_Ramp(Action a, void* payload) {
    switch (a) 
    {
    case Action::ValueL:
    case Action::ValueR: {
        int d = *(int*)payload;
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
        break;
    }
    }
}


static void discrete_rotate(int32_t &value, int32_t d, int32_t num_options) {
    value = (value + d + num_options) % num_options;
}

static void discrete_bounded(int32_t &value, int32_t d, int32_t num_options) {
    int32_t new_value = value + d;
    value = constrain(new_value, 0, num_options - 1);
}

static void notched_bounded(float &value, float d, float step, float min, float max) {
    float new_value = value + d * step;
    value = constrain(new_value, min, max);
}

static void notched_bounded(int32_t &value, int32_t d, int32_t step, int32_t min, int32_t max) {
    int32_t new_value = value + d * step;
    value = constrain(new_value, min, max);
}

static void notched_from_list(float &value, int32_t d, const float *list, size_t list_size) {
    int32_t idx = 0;
    
    float min_dist = 1e9;
    for(int i = 0; i < list_size; i++) {
        float dist = abs(value - list[i]);
        if(dist < min_dist) {
            min_dist = dist;
            idx = i;
        }
    }

    idx = constrain(idx + d, 0, list_size - 1);
    value = list[idx];
}

void AppManager::page_Generator(Action a, void *payload)
{
    static const float AM_FREQ_LIST[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.75, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 7.5, 10.0};
    static const size_t AM_FREQ_LIST_SIZE = sizeof(AM_FREQ_LIST) / sizeof(AM_FREQ_LIST[0]);

    switch (a) 
    {
    case Action::ValueL: {
        int d = *(int*)payload;
        discrete_rotate(state.gen_config_cursor, d, NUM_GEN_CONFIGS);
        break;
    }
    case Action::ValueR: {
        int d = *(int*)payload;
        switch (state.gen_config_cursor)
        {
            case 0:
                // todo
                break;
            case 1:
                notched_bounded(state.gen_config.frequency, d, 50, FREQ_MIN, FREQ_MAX);
                break;
            case 2:
                if(state.gen_config.lr_delta > 0)
                    notched_bounded(state.gen_config.lr_delta, d, 1, 0, 10);
                else if(state.gen_config.lr_delta < 0)
                    notched_bounded(state.gen_config.lr_delta, d, 20, -180, 0);
                else // is zero
                    if(d > 0) state.gen_config.lr_delta = 1;
                    else if (d < 0) state.gen_config.lr_delta = -20;
                break;

            case 3:
                discrete_bounded(state.gen_config.pulse_shape, d, 2);
                break;
            case 4:
                notched_bounded(state.gen_config.pulse_freq, d, 10, 10, 100);
                break;
            case 5:
                notched_bounded(state.gen_config.pulse_bias, d, 10, 10, 100);
                break;

            case 6:
                discrete_bounded(state.gen_config.am_shape, d, 2);
                break;
            case 7:
                notched_from_list(state.gen_config.am_freqency, d, AM_FREQ_LIST, AM_FREQ_LIST_SIZE);
                break;
            case 8:
                notched_bounded(state.gen_config.am_depth, d, 0.1, 0.0, 1);
                break;
        }
        break;
    }
    case Action::PressL: {
        set_l_factor(0);
        break;
    }
    case Action::PressR: {
        set_r_factor(0);
        break;
    }
    case Action::PressM: {
        set_page(Page::Main);
        break;
    }
    case Action::DoublePressM: {
        set_page(Page::Settings);
        break;
    }
    }
}

// ==================
// SETTERS
// ==================
void AppManager::set_page(Page p) {
    if(state.page != p) {
        state.page = p;
        updated = true;
    }
}


void AppManager::set_l_factor(int16_t factor) {
    if (factor < 0) {
        factor = 0;
    } else if (factor > LR_FACTOR_MAX) {
        factor = LR_FACTOR_MAX;
    }

    if(state.l_factor != factor) {
        state.l_factor = factor;

        if(state.r_factor == 0 && state.l_factor == 0) { // both zero means reset
            if(!state.ble_connected) state.vibe_active = false;
        }

        updated = true;
    }
}

void AppManager::set_r_factor(int16_t factor) {
    if (factor < 0) {
        factor = 0;
    } else if (factor > LR_FACTOR_MAX) {
        factor = LR_FACTOR_MAX;
    }

    if(state.r_factor != factor) {
        state.r_factor = factor;

        if(state.r_factor == 0 && state.l_factor == 0) { // both zero means reset
            if(!state.ble_connected) state.vibe_active = false;
        }

        updated = true;
    }
}

void AppManager::set_bt_connected(bool connected) {
    if(state.bt_connected != connected) {
        state.bt_connected = connected;
        updated = true;
    }
}

void AppManager::set_bt_connected_(bool connected) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        set_bt_connected(connected);
        xSemaphoreGive(state_mutex);
    }
    else {
        ESP_LOGE(APPM_TAG, "state acquire fail cannot call: %s", __func__);
    }
}

void AppManager::inc_bt_mode(int d) {
    if(d == 0) return;

    state.bt_mode_cursor = (state.bt_mode_cursor + d + NUM_BTMODE) % NUM_BTMODE;
    updated = true;
}

void AppManager::set_pulse_freq(int freq)
{
    if(freq > PULSE_FREQ_MAX) {
        freq = PULSE_FREQ_MAX;
    }
    else if (freq < PULSE_FREQ_MIN) {
        freq = PULSE_FREQ_MIN;        
    }

    if(state.bt_mode_pulse_freq != freq) {
        state.bt_mode_pulse_freq = freq;
        updated = true;
    }
}

void AppManager::set_pulse_bias(int b)
{
    if(b > PULSE_BIAS_MAX) {
        b = PULSE_BIAS_MAX;
    }
    else if (b < PULSE_BIAS_MIN) {
        b = PULSE_BIAS_MIN;        
    }

    if(state.bt_mode_pulse_bias != b) {
        state.bt_mode_pulse_bias = b;
        updated = true;
    }
}

void AppManager::set_bt_am_freq(int freq) {
    if(freq > FREQ_MAX) {
        freq = FREQ_MAX;
    }
    else if (freq < FREQ_MIN) {
        freq = FREQ_MIN;        
    }

    if(state.bt_mode_am_freq != freq) {
        state.bt_mode_am_freq = freq;
        updated = true;
    }
}


void AppManager::inc_bt_am_shape(int d) {
    if(d == 0) return;

    state.bt_mode_am_shape = (state.bt_mode_am_shape + d + NUM_SHAPES) % NUM_SHAPES;
    updated = true;
}


void AppManager::set_tens_freq(int freq) {
    if(freq > TENS_FREQ_MAX) {
        freq = TENS_FREQ_MAX;
    }
    else if (freq < TENS_FREQ_MIN) {
        freq = TENS_FREQ_MIN;        
    }

    if(state.bt_mode_tens_freq != freq) {
        state.bt_mode_tens_freq = freq;
        updated = true;
    }
}

void AppManager::set_tens_width(int w) {
    if(w > TENS_WIDTH_MAX) {
        w = TENS_WIDTH_MAX;
    }
    else if (w < TENS_WIDTH_MIN) {
        w = TENS_WIDTH_MIN;        
    }

    if(state.bt_mode_tens_width != w) {
        state.bt_mode_tens_width = w;
        updated = true;
    }
}


void AppManager::inc_settings_cursor(int d) {
    if(d == 0) return;

    state.settings_cursor = (state.settings_cursor + d + NUM_SETTINGS) % NUM_SETTINGS;
    updated = true;
}

void AppManager::inc_source_cursor(int d) {
    if(d == 0) return;

    state.source_mode_cursor = (state.source_mode_cursor + d + NUM_SOURCES) % NUM_SOURCES;
    updated = true;
}


void AppManager::set_invert_lr(bool i) {
    if(i != state.settings_invert_lr) {
        state.settings_invert_lr = i;
        updated = true;
    }
}

void AppManager::set_invert_phase(bool i) {
    if(i != state.settings_invert_phase) {
        state.settings_invert_phase = i;
        updated = true;
    }
}

void AppManager::set_ble_connected(bool connected) {
    if(state.ble_connected != connected) {
        // on disconnect
        if(!connected) {
            // state.r_factor = 0;
            // state.l_factor = 0;

            // also zero vibe and remote
            state.vibe_level = 0;
            state.remote_score = 0;

            // only disable vibe if no volume is applied
            if(state.r_factor == 0 && state.l_factor == 0) state.vibe_active = false;
        }
        
        state.ble_connected = connected;
        updated = true;
    }
}

void AppManager::set_ble_connected_(bool connected) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        set_ble_connected(connected);
        xSemaphoreGive(state_mutex);
    }
    else {
        ESP_LOGE(APPM_TAG, "state acquire fail cannot call: %s", __func__);
    }
}

void AppManager::set_vibe_level(int level, bool activate)
{
    if (level < 0) {
        level = 0;
    } else if (level > VIBE_LEVEL_MAX) {
        level = VIBE_LEVEL_MAX;
    }

    if(activate) state.vibe_active = true;

    if(level != state.vibe_level) {
        state.vibe_level = level;
        updated = true;
    }
}

void AppManager::set_vibe_level_(int level, bool activate) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        set_vibe_level(level, activate);
        xSemaphoreGive(state_mutex);
    }
    else {
        ESP_LOGE(APPM_TAG, "state acquire fail cannot call: %s", __func__);
    }
}

void AppManager::set_source_mode(int mode)
{
    if (mode < 0 || mode > NUM_SOURCES) return;

    if(mode != state.source_mode) {
        state.l_factor = 0;  // zero volume on source_change
        state.r_factor = 0;
        state.source_mode = mode;
        updated = true;
    }
}

void AppManager::set_remote_score(int score) {   
    if(score > 1 || score < -1) return;

    if(score != state.remote_score) {
        state.remote_score = score;
        updated = true;
    }
}

void AppManager::set_remote_score_(int score) {
    if(xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        set_remote_score(score);
        xSemaphoreGive(state_mutex);
    }
    else {
        ESP_LOGE(APPM_TAG, "state acquire fail cannot call: %s", __func__);
    }
}
