#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "state/AppManager.h"

class Display_ssd1306 {
protected:
    Adafruit_SSD1306 display;
    State state;

    // PAGES
    void draw_Main();
    void draw_BtMode_Selection();

    void draw_BtMode_Pulse();
    void draw_BtMode_AM();
    void draw_BtMode_Tens();

    void draw_Settings();
    void draw_Settings_Invert();
    void draw_Settings_Ramp();

    // HELPERS
    void draw_text(int16_t x, int16_t y, const char *s, bool top, bool right);
    void draw_text(int16_t x, int16_t y, const char *s, bool right);
    void draw_text(int16_t x, int16_t y, int s, bool top, bool right);
    void draw_2_digit_number(int16_t d1, int16_t d2, int16_t x, int16_t y_btm, int16_t spacing, bool reverse);
    void draw_2_digit_number(int16_t d12, int16_t x, int16_t y_btm, int16_t spacing, bool reverse);

public:
    Display_ssd1306();

    void begin();
    void splash();
    void update(State state);
};