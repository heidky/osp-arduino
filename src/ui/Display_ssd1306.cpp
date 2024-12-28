#include "Display_ssd1306.h"
#include "ui/fonts/FreeSans6pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "icons.h"
#include "Wire.h"


Display_ssd1306::Display_ssd1306() {
    display = Adafruit_SSD1306(128, 64, &Wire);
}


void Display_ssd1306::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    // display.setRotation(2);
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setTextWrap(false);
}


void Display_ssd1306::splash() {
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
}


void Display_ssd1306::update(State state) {
    this->state = state;
    display.clearDisplay();
    // display.drawRect(0, 0, 128, 32, WHITE);

    switch (state.page)
    {
    case Page::Main:
        draw_Main();
        break;
    case Page::BtMode_Selection:
        draw_BtMode_Selection();
        break;
    case Page::BtMode_Pulse:
        draw_BtMode_Pulse();
        break;
    case Page::BtMode_AM:
        draw_BtMode_AM();
        break;
    case Page::BtMode_Tens:
        draw_BtMode_Tens();
        break;
    case Page::Settings:
        draw_Settings();
        break;
    case Page::Settings_Invert:
        draw_Settings_Invert();
        break;
    case Page::Settings_Ramp:
        draw_Settings_Ramp();
        break;        
    }

    display.display();
}


void Display_ssd1306::draw_Main() {
    const uint8_t l_factor = state.l_factor;
    const uint8_t r_factor = state.r_factor;
    const bool bt_connected = state.bt_connected;

    uint8_t l1 = l_factor / 10;
    uint8_t l2 = l_factor % 10;
    uint8_t r1 = r_factor / 10;
    uint8_t r2 = r_factor % 10;

    const bool ble_connected = state.ble_connected;
    const int duck_level = state.duck_level;
    const int vibe_level = state.vibe_level;

    // L, R numbers and center icon
    const int btm = 62;
    display.setFont(&FreeSans18pt7b);

    display.setCursor(0, btm);
    display.print(l1);
    display.setCursor(18+1, btm);
    display.print(l2);

    display.setCursor(127-16-2-16-1, btm);
    display.print(r1);
    display.setCursor(127-16-1, btm);
    display.print(r2);

    display.drawBitmap(128/2-8, btm-18, epd_bitmap_bt_modes[state.bt_mode], 16, 16, WHITE);

    // status indicators
    if(bt_connected) {
        display.fillRect(128/2-8, -1, 16, 6, WHITE);
    } else {
        display.drawRect(128/2-8, -1, 16, 6, WHITE);
    }

    if(ble_connected) {
        display.fillRect(128/4*3-4, -1, 8, 6, WHITE);
    } else {
        display.drawRect(128/4*3-4, -1, 8, 6, WHITE);
    }

    // duck level
    display.setFont(&FreeSans9pt7b);
    display.setCursor(128/4*3-4 + 12, 13);
    display.print(vibe_level);
}


void Display_ssd1306::draw_BtMode_Selection() {
    display.setFont(&FreeSansBold12pt7b);

    // draw_text(6, 20, "B");
    // draw_text(6+16+10, 20, "A");
    // draw_text(6+16+10+16+10, 20, "T");
    for(int i = 0; i < EPD_BITMAP_BT_MODES_NUM; i++)
        display.drawBitmap(6 + (10+16)*i, 4, epd_bitmap_bt_modes[i], 16, 16, WHITE);

    display.drawRect(6 + (10+16) * state.bt_mode_cursor, 32-6, 16, 4, WHITE);
    display.fillRect(6 + (10+16) * state.bt_mode, 32-6, 16, 4, WHITE);
}


void Display_ssd1306::draw_BtMode_Pulse() {
    display.setFont(&FreeSans9pt7b);
    
    String l = String(state.bt_mode_pulse_freq) + "Hz";
    String r = String(state.bt_mode_pulse_bias) + "%";

    draw_text(3, 30, l.c_str(), false, false);
    draw_text(127-3, 30, r.c_str(), false, true);

    // display.setFont(&FreeSans12pt7b);
    // draw_text(127-3, 30, String("Pulse").c_str(), false, true);
}


void Display_ssd1306::draw_BtMode_AM() {
    display.setFont(&FreeSans12pt7b);

    // draw_text(3, 30, state.bt_mode_am_shape, false, false);
    const unsigned char *shape_icon = epd_bitmap_shapes[state.bt_mode_am_shape];
    display.drawBitmap(2, 32/2-6, shape_icon, 16, 16, WHITE);
    String f(state.bt_mode_am_freq);
    draw_text(127-3, 30, (f + "Hz").c_str(), false, true);
}


void Display_ssd1306::draw_BtMode_Tens() {
    display.setFont(&FreeSans9pt7b);

    int wi = roundf(state.bt_mode_tens_width * (1e6f / 44100.f));
    draw_text(3, 30, (String(wi) + "us").c_str(), false, false);
    
    String f(state.bt_mode_tens_freq);
    draw_text(127-3, 30, (f + "Hz").c_str(), false, true);
}

void Display_ssd1306::draw_Settings() {
    display.setFont(&FreeSansBold12pt7b);

    // draw_text(6, 20, "I");
    // draw_text(6+16+10, 20, "R");
    for(int i = 0; i < EPD_BITMAP_SETTINGS_NUM; i++)
        display.drawBitmap(6 + (10+16)*i, 4, epd_bitmap_settings[i], 16, 16, WHITE);

    display.drawRect(6 + (10+16) * state.settings_cursor, 32-6, 16, 4, WHITE);
}

void Display_ssd1306::draw_Settings_Invert() {
    display.setFont(&FreeSans6pt7b);
    draw_text(2, 2, "L/R", true, false);
    draw_text(127-2, 2, "Phase", true, true);

    const char *lr_state = state.settings_invert_lr ? "ON" : "OFF";
    const char *phase_state = state.settings_invert_phase ? "ON" : "OFF";
    
    display.setFont(&FreeSans9pt7b);
    draw_text(2, 31-2, lr_state, false, false);
    draw_text(127-2, 31-2, phase_state, false, true);
}

void Display_ssd1306::draw_Settings_Ramp() {
    display.setFont(&FreeSansBold12pt7b);
    
    draw_text(0, 30, "Ramp");
}


// ====================
// HELPERS
// ====================
void Display_ssd1306::draw_text(int16_t x, int16_t y, int s, bool top, bool right) {
    String s0(s);
    draw_text(x, y, s0.c_str(), top, right);
}

void Display_ssd1306::draw_text(int16_t x, int16_t y, const char* s, bool top, bool right) {
    int16_t x0, y0;
    uint16_t w, h;
    display.getTextBounds(s, 0, 0, &x0, &y0, &w, &h);

    if(right) x -= (w + 2);
    if(top) y += h;

    display.setCursor(x, y);
    display.print(s);
}
