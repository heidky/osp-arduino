#pragma once
#include <Arduino.h>

enum class Action {
    ValueL,
    PressL,

    ValueR,
    PressR,

    PressM,
    DoublePressM,
};

enum class Page {
    Main,

    BtMode_Selection,
    BtMode_Pulse,
    BtMode_AM,
    BtMode_Tens,

    Settings,
    Settings_Invert,
    Settings_Ramp,
};

#define NUM_BTMODE 4
#define BTMODE_BYPASS 0
#define BTMODE_PULSE 1
#define BTMODE_AM 2
#define BTMODE_TENS 3

#define PULSE_FREQ_MIN 10
#define PULSE_FREQ_MAX 100
#define PULSE_FREQ_STEP 5

#define PULSE_BIAS_MIN 5
#define PULSE_BIAS_MAX 100
#define PULSE_BIAS_STEP 5

#define NUM_SHAPES 4
#define SHAPE_SIN 0
#define SHAPE_SQUARE 1
#define SHAPE_TRI 2
#define SHAPE_SAW 3

#define FREQ_STEP 10
#define FREQ_MIN 100
#define FREQ_MAX 1000

#define TENS_WIDTH_STEP 1
#define TENS_WIDTH_MIN 1
#define TENS_WIDTH_MAX 30

#define TENS_FREQ_STEP 10
#define TENS_FREQ_MIN 10
#define TENS_FREQ_MAX 500

#define NUM_SETTINGS 2
#define SETTINGS_INVERT 0
#define SETTINGS_RAMP 1


struct State {
    Page page = Page::Main;
    uint8_t l_factor = 0;
    uint8_t r_factor = 0;

    bool bt_connected = false;
    int bt_mode = BTMODE_BYPASS;
    int bt_mode_cursor = BTMODE_BYPASS;

    int bt_mode_pulse_freq = 50;
    int bt_mode_pulse_bias = 50;

    int bt_mode_am_freq = 400;
    int bt_mode_am_shape = SHAPE_SIN;

    int bt_mode_tens_width = 6;
    int bt_mode_tens_freq = 130;

    int settings_cursor = 0;
    int settings_invert_lr = false;
    int settings_invert_phase = false;

    bool ble_connected = false;
    int duck_level = 0;
    int vibe_level = 0;
    int32_t remote_score = 0;
};

#define LR_FACTOR_MAX 99
#define VIBE_LEVEL_MAX 20


class AppManager {
public:
    bool updated = false;

    State state;

    void set_page(Page p);
    void set_l_factor(int16_t factor);
    void set_r_factor(int16_t factor);

    void set_bt_connected(bool connected);
    void inc_bt_mode(int d);

    void set_pulse_freq(int f);
    void set_pulse_bias(int b);

    void set_bt_am_freq(int freq);
    void inc_bt_am_shape(int d);

    void set_tens_freq(int freq);
    void set_tens_width(int w);

    void inc_settings_cursor(int d);
    void set_invert_lr(bool i);
    void set_invert_phase(bool i);

    void set_ble_connected(bool connected);
    void set_duck_level(int level);
    void set_vibe_level(int level);
    void set_remote_score(int score);

    void action(Action a, void* payload = NULL);

protected:
    void page_Main(Action a, void* payload);

    void page_BtMode_Selection(Action a, void* payload);
    void page_BtMode_Pulse(Action a, void* payload);
    void page_BtMode_AM(Action a, void* payload);
    void page_BtMode_Tens(Action a, void* payload);

    void page_Settings(Action a, void* payload);
    void page_Settings_Invert(Action a, void* payload);
    void page_Settings_Ramp(Action a, void* payload);
};