// ========== GENERAL ============
// #define DEVICE_NAME "Osp2"
#define DEVICE_NAME "LVS-Osp"


// ========== GPIO ============
#define GPIO_ROTARY_L_CLK 18
#define GPIO_ROTARY_L_DT  5
#define GPIO_ROTARY_L_SW  19

#define GPIO_ROTARY_R_CLK 4
#define GPIO_ROTARY_R_DT  15
#define GPIO_ROTARY_R_SW  17

#define GPIO_BUTTON_M 21

#define GPIO_I2S_BCK  27
#define GPIO_I2S_DOUT 26
#define GPIO_I2S_LCK  25

#define GPIO_I2C_SDA  22
#define GPIO_I2C_SCL  23


// ========== BUTTONS ============
#define BTN_DEBOUNCE_MILLIS 3
#define BTN_DOUBLE_PRESS_MILLIS 500
#define BTN_SLEEP_MILLIS 200


// ========== AUDIO PROCESSING =======
#define LR_RISETIME    1.0f
#define VIBE_RISETIME  0.5f

#define REMOTE_BOOST_MULT       1.3f
#define REMOTE_BOOST_RISETIME   120.f
#define REMOTE_BOOST_FALLTIME   30.f
#define REMOTE_DUCK_MULT        0.60f
#define REMOTE_DUCK_RISETIME    1.f
