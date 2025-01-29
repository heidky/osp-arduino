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


void Display_ssd1306::update(const State &state, const UiState &ui_state) {
    this->state = state;
    this->ui_state = ui_state;
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
    case Page::Settings_Source:
        draw_Settings_Source();
        break;
    case Page::Settings_Invert:
        draw_Settings_Invert();
        break;
    case Page::Settings_Ramp:
        draw_Settings_Ramp();
        break;

    case Page::Generator:
        draw_Generator();
        break;
    }

    display.display();
}


void Display_ssd1306::draw_Main() {
    // L, R numbers and center icon
    const int btm = 62;
    display.setFont(&FreeSans18pt7b);
    draw_2_digit_number(state.l_factor, 1,     btm, 19, false);
    draw_2_digit_number(state.r_factor, 127+1, btm, 19, true);
    display.drawBitmap(128/2-8, btm-18, epd_bitmap_bt_modes[state.bt_mode], 16, 16, WHITE);

    // status indicators
    if(state.bt_connected) {
        display.fillRect(128/2-8, -1, 16, 6, WHITE);
    } else {
        display.drawRect(128/2-8, -1, 16, 6, WHITE);
    }

    if(state.ble_connected) {
        display.fillRect(128/2+8+1, -1, 8, 6, WHITE);
    } else {
        display.drawRect(128/2+8+1, -1, 8, 6, WHITE);
    }
    
    // shadow factors
    // display.setFont(&FreeSans12pt7b);
    // draw_2_digit_number(l1, l2, 1,     btm - 27, 13, false);
    // draw_2_digit_number(r1, r2, 127+1, btm - 27, 13, true);

    display.setFont(&FreeSans6pt7b);
    const char *source_text = (state.source_mode == SOURCE_BT) ? "BT" : "Gen";
    draw_text(3, 12, source_text, false);

    const int mod_btm = 16+12;
     // vibe level
    if(state.vibe_active) { 
        display.setFont(&FreeSans6pt7b);
        display.setCursor(128/2-4-8-2, mod_btm);
        display.print('V');
        draw_2_digit_number(state.vibe_level, 128/2-4, mod_btm, 6+1, false);
    }
    // remote state
    else if(state.remote_score != 0) {
        display.setFont(&FreeSans6pt7b);
        if(state.remote_score > 0) 
            draw_text(128/2-8, mod_btm, "UP", false);
        else
            draw_text(128/2-8-2, mod_btm, "DW", false);
    }

    draw_vertical_bar(ui_state.l_level, 64-20,   63, 36, 3, false);
    draw_vertical_bar(ui_state.r_level, 64+20+2, 63, 36, 3, true);
}


void Display_ssd1306::draw_BtMode_Selection() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Mode", false);

    const int16_t top = 32;
    for(int i = 0; i < EPD_BITMAP_BT_MODES_NUM; i++)
        display.drawBitmap(6 + (10+16)*i, top, epd_bitmap_bt_modes[i], 16, 16, WHITE);

    display.drawRect(6 + (10+16) * state.bt_mode_cursor,  top+16+6, 16, 4, WHITE);
    display.fillRect(6 + (10+16) * state.bt_mode,         top+16+6, 16, 4, WHITE);
}


void Display_ssd1306::draw_BtMode_Pulse() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Freq", false);
    draw_text(127-3, 12, "Bias", true);

    display.setFont(&FreeSans9pt7b);
    String l = String(state.bt_mode_pulse_freq) + "Hz";
    String r = String(state.bt_mode_pulse_bias) + "%";

    draw_text(3, 30, l.c_str(), false, false);
    draw_text(127-3, 30, r.c_str(), false, true);

    // display.setFont(&FreeSans12pt7b);
    // draw_text(127-3, 30, String("Pulse").c_str(), false, true);
}


void Display_ssd1306::draw_BtMode_AM() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Shape", false);
    draw_text(127-3, 12, "Freq", true);

    display.setFont(&FreeSans12pt7b);
    // draw_text(3, 30, state.bt_mode_am_shape, false, false);
    const unsigned char *shape_icon = epd_bitmap_shapes[state.bt_mode_am_shape];
    display.drawBitmap(3, 18+4, shape_icon, 16, 16, WHITE);
    String f(state.bt_mode_am_freq);
    draw_text(127-3, 32+4, (f + "Hz").c_str(), false, true);
}


void Display_ssd1306::draw_BtMode_Tens() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Width", false);
    draw_text(127-3, 12, "Freq", true);

    display.setFont(&FreeSans9pt7b);
    int wi = roundf(state.bt_mode_tens_width * (1e6f / 44100.f));
    draw_text(3, 30, (String(wi) + "us").c_str(), false, false);
    
    String f(state.bt_mode_tens_freq);
    draw_text(127-3, 30, (f + "Hz").c_str(), false, true);
}

void Display_ssd1306::draw_Settings() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Settings", false);

    const int16_t top = 32;
    for(int i = 0; i < EPD_BITMAP_SETTINGS_NUM; i++)
        display.drawBitmap(6 + (10+16)*i, top, epd_bitmap_settings[i], 16, 16, WHITE);

    display.drawRect(6 + (10+16) * state.settings_cursor, top+16+6, 16, 4, WHITE);
}

void Display_ssd1306::draw_Settings_Source() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "Source Mode", false);
    const int16_t top = 32;

    for(int i = 0; i < EPD_BITMAP_SOURCES_NUM; i++)
        display.drawBitmap(6 + (10+16)*i, top, epd_bitmap_sources[i], 16, 16, WHITE);

    display.drawRect(6 + (10+16) * state.source_mode_cursor, top+16+6, 16, 4, WHITE);
    display.fillRect(6 + (10+16) * state.source_mode,        top+16+6, 16, 4, WHITE);
}

void Display_ssd1306::draw_Settings_Invert() {
    display.setFont(&FreeSans6pt7b);
    draw_text(3, 12, "L/R", false);
    draw_text(127-3, 12, "Phase", true);

    const char *lr_state = state.settings_invert_lr ? "ON" : "OFF";
    const char *phase_state = state.settings_invert_phase ? "ON" : "OFF";
    
    display.setFont(&FreeSans9pt7b);
    draw_text(2, 31-2, lr_state, false);
    draw_text(127-2, 31-2, phase_state, true);
}

void Display_ssd1306::draw_Settings_Ramp() {
    display.setFont(&FreeSansBold12pt7b);
    
    display.setCursor(4, 42);
    display.print("Ramp~WIP");
}

void Display_ssd1306::draw_Generator()
{   
    const int16_t items_per_page = 3;
    const int16_t num_pages = NUM_GEN_CONFIGS / items_per_page;
    const int16_t page = state.gen_config_cursor / items_per_page;

    // TITLE
    display.setFont(&FreeSans6pt7b);
    display.setCursor(0, 12);
    display.print("Generator [");
    display.print(page + 1);
    display.print("/");
    display.print(num_pages);
    display.print("]");

    // ROWS
    static const char *labels[] = {"Shape", "Freq", "LR Delta", "PL Shape", "PL Freq", "PL Bias", "AM Shape", "AM Freq", "AM Depth"};
    static const char *OFF = "OFF";
    static const char *ON = "ON";  

    display.setFont(&FreeSans6pt7b);

    for(int i = 0; i < items_per_page; i++) {
        const int16_t mid_y = 17 + 2 + 16*i + 8;

        // CHECK BOX CURSOR
        int16_t idx = page * items_per_page + i;
        display.drawRoundRect(0, mid_y-4, 8, 8, 4, WHITE);
        if(state.gen_config_cursor == idx)
            display.fillRoundRect(0+2, mid_y-4+2, 8-4, 8-4, 4, WHITE);

        // LABEL
        display.setCursor(8+2, mid_y+3);
        display.print(labels[idx]);

        // VALUE
    char value_text[32] = {0};
        switch (idx)
        {
            case 0:
                snprintf(value_text, sizeof(value_text), "SIN");
                break;
            case 1:
                snprintf(value_text, sizeof(value_text), "%dHz", (int16_t)state.gen_config.frequency);
                break;
            case 2:
                if(state.gen_config.lr_delta >= 0)
                    snprintf(value_text, sizeof(value_text), "+%dHz", state.gen_config.lr_delta);
                else
                    snprintf(value_text, sizeof(value_text), "%ddeg", -state.gen_config.lr_delta);
                break;

            case 3:
                snprintf(value_text, sizeof(value_text), "%s", state.gen_config.pulse_shape ? ON : OFF);
                break;
            case 4:
                snprintf(value_text, sizeof(value_text), "%dHz", state.gen_config.pulse_freq);
                break;
            case 5:
                snprintf(value_text, sizeof(value_text), "%d%", state.gen_config.pulse_bias);
                break;

            case 6:
                snprintf(value_text, sizeof(value_text), "%s", state.gen_config.am_shape ? ON : OFF);
                break;
            case 7:
                snprintf(value_text, sizeof(value_text), "%.1fHz", state.gen_config.am_freqency);
                break;
            case 8:
                snprintf(value_text, sizeof(value_text), "%.1f", state.gen_config.am_depth);
                break;
        }

        draw_text(128, mid_y+3, value_text, true);
    }
}

// ====================
// HELPERS
// ====================
void Display_ssd1306::draw_text(int16_t x, int16_t y, int s, bool top, bool right) {
    String s0(s);
    draw_text(x, y, s0.c_str(), top, right);
}

void Display_ssd1306::draw_text(int16_t x, int16_t y, const char* s, bool right) {
    int16_t x0, y0;
    uint16_t w, h;
    display.getTextBounds(s, 0, 0, &x0, &y0, &w, &h);

    if(right) x -= (w + 2);

    display.setCursor(x, y);
    display.print(s);
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

void Display_ssd1306::draw_2_digit_number(int16_t d1, int16_t d2, int16_t x, int16_t y_btm, int16_t spacing, bool reverse) {
    if(!reverse) {
        display.setCursor(x, y_btm);
        display.print(d1);
        display.setCursor(x + spacing, y_btm);
        display.print(d2);
    }
    else {
        display.setCursor(x - spacing*2, y_btm);
        display.print(d1);
        display.setCursor(x - spacing, y_btm);
        display.print(d2);
    }
}

void Display_ssd1306::draw_2_digit_number(int16_t d12, int16_t x, int16_t y_btm, int16_t spacing, bool reverse) {
    uint8_t d1 = d12 / 10;
    uint8_t d2 = d12 % 10;
    draw_2_digit_number(d1, d2, x, y_btm, spacing, reverse);
}

void Display_ssd1306::draw_vertical_bar(float perc, int16_t x, int16_t y_btm, int16_t y_top, int16_t w, bool x_right_align)
{
    if(perc < 1e-4f) return;
    const int16_t x_left = x_right_align ? x - w : x;
    const int16_t x_line = x_right_align ? x - 1 : x;
    int16_t y_fill = y_btm + (y_top - y_btm) * perc;
    display.fillRect(x_left, y_fill, w,  (y_btm-y_fill), WHITE);
    display.drawFastVLine(x_line, y_top, y_btm - y_top, WHITE);
    display.drawFastHLine(x_left, y_top, w, WHITE);
    // display.drawRect(x, y_top,  w,  1,              WHITE);
}
