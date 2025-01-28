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
    Settings_Source,
    Settings_Invert,
    Settings_Ramp,

    Generator,
};

#define LR_FACTOR_MAX 99
#define VIBE_LEVEL_MAX 20

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

#define NUM_SETTINGS 3
#define SETTINGS_SOURCE 0
#define SETTINGS_INVERT 1
#define SETTINGS_RAMP 2

#define NUM_SOURCES 2
#define SOURCE_BT 0
#define SOURCE_GEN 1

#define NUM_GEN_CONFIGS 9

struct SoundGeneratorConfig {
    float frequency = 400; // [200-1000]
    int32_t shape = 1; 
    int32_t lr_delta = 0; // < 0 is phase [0-180], >= 0 frequency delta*50 [0, 100] 
    // float lr_phase_offset = 0;

    int32_t pulse_shape = 0;
    int32_t pulse_freq = 50; // [10-100]
    int32_t pulse_bias = 50; // [10-100]

    int32_t am_shape = 0; 
    float am_freqency = 1;  // [0.1-10]
    float am_depth = 0;     //[0-100]
};

struct State {
    Page page = Page::Main;
    uint8_t l_factor = 0;
    uint8_t r_factor = 0;

    int source_mode = SOURCE_BT;
    int source_mode_cursor = SOURCE_BT;

    bool bt_connected = false;
    int bt_mode = BTMODE_BYPASS;
    int bt_mode_cursor = BTMODE_BYPASS;

    int bt_mode_pulse_freq = 50;
    int bt_mode_pulse_bias = 50;

    int bt_mode_am_freq = 400;
    int bt_mode_am_shape = SHAPE_SIN;

    int bt_mode_tens_width = 6;
    int bt_mode_tens_freq = 130;

    // SETTINGS
    int settings_cursor = 0;
    int settings_invert_lr = false;
    int settings_invert_phase = false;

    // BLE
    bool ble_connected = false;
    int vibe_level = 0;
    bool vibe_active = false;
    int32_t remote_score = 0;

    // GENERATOR
    SoundGeneratorConfig gen_config;
    int32_t gen_config_cursor = 3;
};


struct UiState {
    float r_level;
    float l_level;
};


class AppManager {
public:
    AppManager();

    bool updated = false;

    UiState ui_state;

    void action(Action a, void* payload = NULL);
    bool copy_state_into(State &out_state);

protected:
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
    void set_vibe_level(int level, bool activate=false);
    void set_remote_score(int score);

    void inc_source_cursor(int d);
    void set_source_mode(int mode);

public:
    void set_bt_connected_(bool connected);
    void set_ble_connected_(bool connected);
    void set_vibe_level_(int level, bool activate=false);
    void set_remote_score_(int score);

protected:
    SemaphoreHandle_t state_mutex;
    State state;

    void page_Main(Action a, void* payload);

    void page_BtMode_Selection(Action a, void* payload);
    void page_BtMode_Pulse(Action a, void* payload);
    void page_BtMode_AM(Action a, void* payload);
    void page_BtMode_Tens(Action a, void* payload);

    void page_Settings(Action a, void* payload);
    void page_Settings_Source(Action a, void* payload);
    void page_Settings_Invert(Action a, void* payload);
    void page_Settings_Ramp(Action a, void* payload);

    void page_Generator(Action a, void* payload);
};