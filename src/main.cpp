#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MenuItem.h>

#define LINE_SPACING_PX 3
#define FONT_HEIGHT_PX 13
#define FONT_GLYPH_LEG_PX 2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, 4, 3);
uint32_t last_draw = millis();

static uint8_t cursor = 0;
int temp = 437;
int time_m = 1200;
int hold_temp = 400;
int hold_time = 600;

menu_item_t menu_items[] = {
  {"Temp.", MENU_ITEM_TYPE_VALUE},
  {"Time", MENU_ITEM_TYPE_VALUE},
  {"Hold temp.", MENU_ITEM_TYPE_VALUE},
  {"Hold time", MENU_ITEM_TYPE_VALUE}
};

menu_screen_t main_screen = {
  .id = 0,
  .title = "Main Menu",
  .items = menu_items,
  .item_count = sizeof(menu_items) / sizeof(menu_item_t)
};

void drawTitle(menu_screen_t screen) {
  u8g2.setFont(u8g2_font_t0_13b_tf);
  u8g2.drawUTF8(0,FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX, screen.title);
  u8g2.drawLine(0, FONT_HEIGHT_PX + 1, 128, FONT_HEIGHT_PX + 1);
}

void drawItem(menu_item_t item, bool selected, uint8_t line) {
  u8g2.setFont(u8g2_font_t0_13b_tf);
  u8g2.drawUTF8(8, (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 2, item.label);
  if (item.type == MENU_ITEM_TYPE_VALUE) {
    char value_str[10];
    dtostrf(*(item.target.value_ptr)/10.0, 0, 1, value_str);
    u8g2.drawUTF8(128 - u8g2.getUTF8Width(value_str), (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 2, value_str);
  }
  if (selected) {
    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawGlyph(0, (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 1, 0x25b6);
    u8g2.setDrawColor(2);
    u8g2.drawBox(0, (line) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 1 , 128, FONT_HEIGHT_PX + LINE_SPACING_PX - 1);
  }
}

void drawScreen(menu_screen_t screen, uint8_t cursor) {
  drawTitle(screen);
  if (cursor == screen.item_count-1) {
    drawItem(screen.items[screen.item_count-3], false, 1);
    drawItem(screen.items[screen.item_count-2], false, 2);
    drawItem(screen.items[screen.item_count-1], true, 3);
  } else if (cursor == 0) {
    drawItem(screen.items[0], true, 1);
    drawItem(screen.items[1], false, 2);
    drawItem(screen.items[2], false, 3);
  } else {
    for (int i = 0; i < 3; i++) {
      drawItem(screen.items[cursor+i-1], i == 1, i+1);
    }
  }
}

void setup() {
  menu_items[0].target.value_ptr = &temp;
  menu_items[1].target.value_ptr = &time_m;
  menu_items[2].target.value_ptr = &hold_temp;
  menu_items[3].target.value_ptr = &hold_time;

  u8g2.setI2CAddress(0x7A);
  u8g2.begin();
}

void loop() {
  uint32_t now = millis();
  if (now - last_draw > 1500) {
    last_draw = now;
    u8g2.clearBuffer();
    drawScreen(main_screen, cursor);
    u8g2.sendBuffer();
    cursor = (cursor + 1) % main_screen.item_count;
  }
}
