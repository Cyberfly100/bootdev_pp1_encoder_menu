#pragma once
#include <Arduino.h>
#include "MenuItem.h"
#include "Stack.h"

class MenuNavigator {
public:
    void init(const menu_screen_t* screens, uint8_t screen_count);
    void onRotate(int8_t delta);
    void onConfirm();
    void onBack();
    bool isDirty();
    
    const menu_screen_t* getCurrentScreen() const;
    uint8_t getCursor() const;
    bool isEditingValue() const;
    void setActive(bool active);
private:
    bool _active;
    const menu_screen_t* _screens;
    uint8_t _screen_count;
    stack_t* _stack;
    uint8_t _current_screen_index;
    uint8_t _cursor;
    bool _editing;
    int _edit_temp;
    bool _dirty; // redraw needed
};