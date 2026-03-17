#pragma once
#include <Arduino.h>

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

typedef union MenuItemTarget {
    void (*action)();
    uint8_t target_screen;
    int* value_ptr;
} menu_item_target_t;


typedef struct MenuItem {
    const char* label;
    menu_item_type_t type;
    value_type_t value_type;
    menu_item_target_t target;
} menu_item_t;

typedef struct MenuScreen {
    uint8_t id;
    const char* title;
    const menu_item_t* items;
    size_t item_count;
} menu_screen_t;