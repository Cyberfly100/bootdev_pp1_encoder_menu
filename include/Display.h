#pragma once
#include <U8g2lib.h>
#include "MenuItem.h"
#include <logo.h>
#include <esp32-hal-log.h>
#include "MenuNavigator.h"

class DisplayRenderer {
public:
    DisplayRenderer(U8G2& display, MenuNavigator& navigator);

    void initialize();
    
    void renderHome(int current_temp, int target_temp, int target_duration, int runtime_m);
    void renderMenu(const menu_screen_t* screen);
    void renderLogo();
    
    void clear();
    void display();
    
private:
    void drawTitle(const menu_screen_t* screen);
    void drawItem(const menu_item_t* item, uint8_t line, bool selected, bool editing);
    void formatValue(char* buffer, const menu_item_t* item, bool editing);
    u8g2_uint_t getStrWidth(const char* str, int uptochar = -1);
    
    U8G2& _display;
    MenuNavigator& _navigator;
    static const uint8_t LINE_SPACING_PX = 3;
    static const uint8_t FONT_HEIGHT_PX = 13;
    static const uint8_t FONT_GLYPH_LEG_PX = 2;
};