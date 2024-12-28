#include "AppManager.h"

void AppManager::action(Action a, void* payload) {
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
    case Page::Settings_Invert:
        page_Settings_Invert(a, payload);
        break;
    case Page::Settings_Ramp:
        page_Settings_Ramp(a, payload);
        break;
    }
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
        set_page(Page::BtMode_Selection);
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
        if(s == SETTINGS_INVERT) {
            set_page(Page::Settings_Invert);
        }
        else if(s == SETTINGS_RAMP) {
            set_page(Page::Settings_Ramp);
        }
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
    } else if (factor > 99) {
        factor = 99;
    }

    if(state.l_factor != factor) {
        state.l_factor = factor;
        updated = true;
    }
}

void AppManager::set_r_factor(int16_t factor) {
    if (factor < 0) {
        factor = 0;
    } else if (factor > 99) {
        factor = 99;
    }

    if(state.r_factor != factor) {
        state.r_factor = factor;
        updated = true;
    }
}

void AppManager::set_bt_connected(bool connected) {
    if(state.bt_connected != connected) {
        state.bt_connected = connected;
        updated = true;
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
        state.ble_connected = connected;

        if(!connected) {
            state.r_factor = 0;
            state.l_factor = 0;
        }
        
        updated = true;
    }
}

void AppManager::set_duck_level(int level)
{
    if(level != state.duck_level) {
        state.duck_level = level;
        updated = true;
    }
}

void AppManager::set_vibe_level(int level)
{
    if(level != state.vibe_level) {
        state.vibe_level = level;
        updated = true;
    }
}