#include <MenuNavigator.h>

MenuNavigator::MenuNavigator() : _stack(NULL) {}

MenuNavigator::~MenuNavigator() {
    if (_stack != NULL) {
        stack_free(_stack);
        _stack = NULL;
    }
}

void MenuNavigator::init(const menu_screen_t* screens, uint8_t screen_count){
    _active = false;
    _edit_position = 0;
    _screens = screens;
    _screen_count = screen_count;
    _stack = stack_new(4, STACK_TYPE_INT); // stack autoresizes
    _current_screen_index = 0;
    _cursor = 0;
    _editing = false;
    _edit_val_int = 0;
    _edit_val_text[0] = '\0';
    _dirty = true; // Initial draw needed
    if (_stack) {
        stack_push_int(_stack, _current_screen_index);
    }
}

void MenuNavigator::onRotate(int8_t delta) {
    if (delta == 0) {
        return;
    }
    if (_editing) {
        // editing value, not moving cursor.
        const menu_screen_t* current_screen = getCurrentScreen();
        if (current_screen && _cursor < current_screen->item_count) {
            const menu_item_t* current_item = &current_screen->items[_cursor];
            if (current_item->type == MENU_ITEM_TYPE_VALUE) {
                switch (current_item->value_type_info->value_type) {
                    case VALUE_TYPE_TEMP:
                        _edit_val_int += delta;
                        break;
                    case VALUE_TYPE_TIME:
                        if (_edit_position == 0) {
                            _edit_val_int += delta * 60; // change hours
                            if (_edit_val_int > 23 * 60 + 59) {
                                _edit_val_int = 0;
                            }
                        } else {
                            _edit_val_int += delta; // change minutes
                        }
                        if (_edit_val_int < 0) {
                            _edit_val_int = 0;
                        }
                        break;
                    case VALUE_TYPE_TEXT:
                        // cycle through ASCII chars 32-126
                        _edit_val_int += delta;
                        if (_edit_val_int < 32) {
                            _edit_val_int = 126;
                        } else if (_edit_val_int > 126) {
                            _edit_val_int = 32;
                        }
                        break;
                    default:
                        _edit_val_int += delta;
                         break;
                }
                _dirty = true;
            }
        }
    } else {
        const menu_screen_t* current_screen = getCurrentScreen();
        if (!current_screen || current_screen->item_count == 0) {
            return;
        }

        int prev_cursor = _cursor;
        int new_cursor = (int)_cursor + delta;

        if (new_cursor >= (int)current_screen->item_count) {
            _cursor = current_screen->item_count - 1;
        } else if (new_cursor < 0) {
            _cursor = 0;
        } else {
            _cursor = new_cursor;
        }

        if (prev_cursor != _cursor) {
            _dirty = true;
        }
    }
}

void MenuNavigator::onConfirm() {
    log_i("MenuNavigator::onConfirm cursor=%d active=%d editing=%d", _cursor, _active, _editing);
    const menu_screen_t* current_screen = getCurrentScreen();
    if (!current_screen || _cursor >= current_screen->item_count) {
        log_w("MenuNavigator::onConfirm abort invalid screen/cursor");
        return;
    }
    
    const menu_item_t* current_item = &current_screen->items[_cursor];

    switch (current_item->type) {
        case MENU_ITEM_TYPE_ACTION:
            if (current_item->target.action != NULL) {
                current_item->target.action();
            }
            break;
        case MENU_ITEM_TYPE_NAVIGATE:
            stack_push_int(_stack, _current_screen_index);
            _current_screen_index = current_item->target.target_screen;
            _cursor = 0;
            break;
        case MENU_ITEM_TYPE_VALUE:
            if (current_item->value_type_info == NULL) {
                return;
            }
            if (_editing) {
                if (current_item->value_type_info->value_type == VALUE_TYPE_TEXT) {
                    _edit_val_text[_edit_position] = (char)_edit_val_int;
                }
                _edit_position++;
                if (current_item->value_type_info->value_type == VALUE_TYPE_TEXT) {
                    _edit_val_int = _edit_val_text[_edit_position];
                    if (_edit_position >= current_item->value_type_info->edit_positions) {
                        if (current_item->target.value.text_value) {
                            int last_char = strlen(_edit_val_text) - 1;
                            while (last_char >= 0 && _edit_val_text[last_char] == ' ') {
                                last_char--;
                            }
                            _edit_val_text[last_char + 1] = '\0';
                            strncpy(current_item->target.value.text_value, _edit_val_text, MAX_STRING_LENGTH);
                        }
                        _editing = false;
                        _edit_position = 0;
                    }
                } else {
                    if (_edit_position >= current_item->value_type_info->edit_positions) {
                        if (current_item->target.value.int_value) {
                            *(current_item->target.value.int_value) = _edit_val_int;
                        }
                        _editing = false;
                        _edit_position = 0;
                    }
                }
            } else {
                if (current_item->value_type_info->value_type == VALUE_TYPE_TEXT) {
                    if (current_item->target.value.text_value) {
                        strncpy(_edit_val_text, current_item->target.value.text_value, sizeof(_edit_val_text) - 1);
                        for (size_t i = strlen(_edit_val_text); i < MAX_STRING_LENGTH; i++) {
                            _edit_val_text[i] = ' ';
                        }
                        _edit_val_text[sizeof(_edit_val_text) - 1] = '\0';
                        _edit_val_int = _edit_val_text[_edit_position];
                        _editing = true;
                        _edit_position = 0;
                    }
                } else {
                    if (current_item->target.value.int_value) {
                        _edit_val_int = *(current_item->target.value.int_value);
                        _editing = true;
                        _edit_position = 0;
                    }
                }
            }
            break;
    }
    _dirty = true;
}

void MenuNavigator::onBack() {
    if (_editing) {
        _edit_position--;
        const menu_screen_t* current_screen = getCurrentScreen();
        const menu_item_t* current_item = &current_screen->items[_cursor];
        if (_edit_position < 0) {
            _editing = false;
            _edit_position = 0;
        } else if (current_item->value_type_info->value_type == VALUE_TYPE_TEXT) {
            _edit_val_int = _edit_val_text[_edit_position];
        }
    } else if (_stack && _stack->count > 0) {
        _current_screen_index = stack_pop_int(_stack);
        _cursor = 0;

        if (_stack->count == 0) {
            _active = false; // return home
        }
    }
    _dirty = true;
}

bool MenuNavigator::isDirty(){
    bool was_dirty = _dirty;
    _dirty = false;
    return was_dirty;
}

const menu_screen_t* MenuNavigator::getCurrentScreen() const {
    if (!_screens || _current_screen_index >= _screen_count) {
        return NULL;
    }
    return &_screens[_current_screen_index];
}

uint8_t MenuNavigator::getCursor() const {
    return _cursor;
}

void MenuNavigator::setCursor(uint8_t cursor) {
    _cursor = cursor;
}

bool MenuNavigator::isEditingValue() const {
    return _editing;
}

uint8_t MenuNavigator::getEditPosition() const {
    return _edit_position;
}

int MenuNavigator::getEditInt() const {
    return _edit_val_int;
}

const char* MenuNavigator::getEditText() const {
    return _edit_val_text;
}

void MenuNavigator::setActive(bool active) {
    if (active) {
        _dirty = true;
        _cursor = 0;
        _editing = false;
        _current_screen_index = 0;
        _edit_val_int = 0;
        _edit_val_text[0] = '\0';

        if (!_stack) {
            _stack = stack_new(4, STACK_TYPE_INT);
        }

        if (_stack) {
            stack_clear(&_stack);
            stack_push_int(_stack, _current_screen_index);
        }
    }
    _active = active;

}

bool MenuNavigator::isActive() const {
    return _active;
}