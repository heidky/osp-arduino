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

    void draw_Main();
    void draw_BtMode_Selection();
    void draw_BtMode_AM();
    void draw_BtMode_Tens();

    void draw_Settings();
    void draw_Settings_Invert();
    void draw_Settings_Ramp();

    void draw_text(int16_t x, int16_t y, const char *s, bool top = false, bool right = false);
    void draw_text(int16_t x, int16_t y, int s, bool top = false, bool right = false);

public:
    Display_ssd1306();

    void begin();
    void splash();
    void update(State state);
};