#pragma once
#include <Arduino.h>

#define MAX_STRING_LENGTH 10

typedef enum ScreenID {
  SCR_MAIN_MENU = 0,
  SCR_RECIPES,
  SCR_BEEF_STEAK_RARE,
  SCR_BEEF_STEAK_MEDIUM_RARE,
  SCR_BEEF_STEAK_WELL_DONE,
  SCREEN_COUNT
} screenid_t;

typedef enum ValueType {
    VALUE_TYPE_INT,
    VALUE_TYPE_TEMP,
    VALUE_TYPE_TIME,
    VALUE_TYPE_TEXT
} value_type_t;

typedef enum MenuItemType {
    MENU_ITEM_TYPE_ACTION,
    MENU_ITEM_TYPE_NAVIGATE,
    MENU_ITEM_TYPE_VALUE
} menu_item_type_t;

typedef union MenuItemValue {
    int* int_value;
    char* text_value;
} menu_item_value_t;

typedef union MenuItemTarget {
    void (*action)();
    screenid_t target_screen;
    menu_item_value_t value;
} menu_item_target_t;

typedef struct ValueTypeInfo {
    value_type_t value_type;
    uint8_t edit_positions;
} value_type_info_t;


typedef struct MenuItem {
    const char* label;
    menu_item_type_t type;
    value_type_info_t* value_type_info;
    menu_item_target_t target;
} menu_item_t;

typedef struct MenuScreen {
    const char* title;
    const menu_item_t* items;
    size_t item_count;
} menu_screen_t;

static value_type_info_t temp_info = {VALUE_TYPE_TEMP, 1};
static value_type_info_t time_info = {VALUE_TYPE_TIME, 2};
static value_type_info_t string_info = {VALUE_TYPE_TEXT, MAX_STRING_LENGTH};