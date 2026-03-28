#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MenuItem.h>
#include <esp32-hal-log.h>
#include <MenuNavigator.h>
#include <Display.h>
#include <Encoder.h>

// #define LINE_SPACING_PX 3
// #define FONT_HEIGHT_PX 13
// #define FONT_GLYPH_LEG_PX 2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, 4, 3);

MenuNavigator navigator;
DisplayRenderer displayRenderer(u8g2, navigator);
uint32_t last_draw = millis();

int temp = 437;
int time_m = 120;
int hold_temp = 400;
int hold_time = 60;
char test_string[MAX_STRING_LENGTH + 1] = "name";


menu_item_value_t text_value = {.text_value = test_string};

menu_item_value_t temp_value = {.int_value = &temp};
menu_item_value_t time_value = {.int_value = &time_m};
menu_item_value_t hold_temp_value = {.int_value = &hold_temp};
menu_item_value_t hold_time_value = {.int_value = &hold_time};

const menu_item_t beef_steak_rare_params[] = {
  {"Toggle light", MENU_ITEM_TYPE_ACTION, NULL, {.action = Encoder::toggle_light}},
  {"Name", MENU_ITEM_TYPE_VALUE, &string_info, {.value = text_value}},
  {"Temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = temp_value}},
  {"Time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = time_value}},
  {"Hold temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = hold_temp_value}},
  {"Hold time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = hold_time_value}}
};

const menu_item_t beef_steak_mediumrare_params[] = {
  {"Temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = temp_value}},
  {"Time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = time_value}},
  {"Hold temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = hold_temp_value}},
  {"Hold time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = hold_time_value}}
};

const menu_item_t beef_steak_well_done_params[] = {
  {"Temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = temp_value}},
  {"Time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = time_value}},
  {"Hold temp.", MENU_ITEM_TYPE_VALUE, &temp_info, {.value = hold_temp_value}},
  {"Hold time", MENU_ITEM_TYPE_VALUE, &time_info, {.value = hold_time_value}}
};

const menu_item_t beef_steak_items[] = {
  {"Beef steak rare", MENU_ITEM_TYPE_NAVIGATE, NULL, {.target_screen = SCR_BEEF_STEAK_RARE}},
  {"Beef steak medium rare", MENU_ITEM_TYPE_NAVIGATE, NULL, {.target_screen = SCR_BEEF_STEAK_MEDIUM_RARE}},
  {"Beef steak well done", MENU_ITEM_TYPE_NAVIGATE, NULL, {.target_screen = SCR_BEEF_STEAK_WELL_DONE}}
};

const menu_item_t main_menu[] = {
  {"Recipes", MENU_ITEM_TYPE_NAVIGATE, NULL, {.target_screen = SCR_RECIPES}}
};

menu_screen_t screens[SCREEN_COUNT] = {
    [SCR_MAIN_MENU] = {
        .title = "Main Menu",
        .items = main_menu,
        .item_count = sizeof(main_menu) / sizeof(menu_item_t)
    },
    [SCR_RECIPES] = {
        .title = "Recipes",
        .items = beef_steak_items,
        .item_count = sizeof(beef_steak_items) / sizeof(menu_item_t)
    },
    [SCR_BEEF_STEAK_RARE] = {
        .title = "Beef steak rare",
        .items = beef_steak_rare_params,
        .item_count = sizeof(beef_steak_rare_params) / sizeof(menu_item_t)
    },
    [SCR_BEEF_STEAK_MEDIUM_RARE] = {
        .title = "Beef steak medium rare",
        .items = beef_steak_mediumrare_params,
        .item_count = sizeof(beef_steak_mediumrare_params) / sizeof(menu_item_t)
    },
    [SCR_BEEF_STEAK_WELL_DONE] = {
        .title = "Beef steak well done",
        .items = beef_steak_well_done_params,
        .item_count = sizeof(beef_steak_well_done_params) / sizeof(menu_item_t)
    }
};

void setup() {
  navigator.init(screens, SCREEN_COUNT);
  
  Serial.begin(115200);
  Encoder::Setup();

  displayRenderer.initialize();
  displayRenderer.renderLogo();
  delay(800);
}

void loop() {

  Encoder::Control();
  uint32_t now = millis();
  if (now - last_draw > 100) {
    last_draw = now;

    int live_temp = random(427, 447);

    if (Encoder::get_encoder_delta() != 0) {
      if (!navigator.isActive()) {
        navigator.setActive(true);
      } else {
        navigator.onRotate(Encoder::get_encoder_delta());
      }
    }
    Encoder::button_pressed_type_t button_press_type = Encoder::get_button_pressed();
    switch (button_press_type) {
      case Encoder::SHORT:
        navigator.onConfirm();
        break;
      case Encoder::LONG:
        navigator.onBack();
        break;
      default:
        break;
    }
    if (navigator.isDirty() && navigator.isActive()) {
      displayRenderer.renderMenu(navigator.getCurrentScreen());
    } else if (!navigator.isActive()) {
      displayRenderer.renderHome(live_temp, temp, time_m, (int)round(millis()/1000/60));
    }
    // u8g2.clearBuffer();
    // drawScreen(&main_screen, cursor);
    // u8g2.sendBuffer();
    // cursor = (cursor + 1) % main_screen.item_count;
  }
}
