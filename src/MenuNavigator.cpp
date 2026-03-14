#include <MenuNavigator.h>

void MenuNavigator::init(const menu_screen_t* screens, uint8_t screen_count){
    _active = false;
    _screens = screens;
    _screen_count = screen_count;
    _stack = stack_new(4); // stack autoresizes
    _current_screen_index = 0;
    _cursor = 0;
    _editing = false;
    _edit_temp = 0;
    _dirty = true; // Initial draw needed
    stack_push(_stack, _current_screen_index);
}

void MenuNavigator::onRotate(int8_t delta) {
    if (delta == 0) {
        return;
    }
    if (_editing) {
        // editing value, not moving cursor.
        const menu_screen_t* current_screen = getCurrentScreen();
        const menu_item_t* current_item = &current_screen->items[_cursor];
        if (current_item->type == MENU_ITEM_TYPE_VALUE) {
            _edit_temp += delta;
            _dirty = true;
        }
    } else {
        const menu_screen_t* current_screen = getCurrentScreen();
        int prev_cursor = _cursor;
        int new_cursor = _cursor + delta;
        if (new_cursor >= current_screen->item_count) {
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
    const menu_screen_t* current_screen = getCurrentScreen();
    const menu_item_t* current_item = &current_screen->items[_cursor];

    switch (current_item->type) {
        case MENU_ITEM_TYPE_ACTION:
            if (current_item->target.action != NULL) {
                current_item->target.action();
            }
            break;
        case MENU_ITEM_TYPE_NAVIGATE:
            stack_push(_stack, _current_screen_index);
            _current_screen_index = current_item->target.target_screen;
            _cursor = 0;
            _dirty = true;
            break;
        case MENU_ITEM_TYPE_VALUE:
            if (_editing) {
                // Stop editing and save value
                if (current_item->target.value_ptr != NULL) {
                    *(current_item->target.value_ptr) = _edit_temp;
                    _dirty = true;
                }
                _editing = false;
            } else {
                // Start editing the value
                if (current_item->target.value_ptr != NULL) {
                    _edit_temp = *(current_item->target.value_ptr);
                    _editing = true;
                    _dirty = true;
                }
            }
            break;
    }
}

void MenuNavigator::onBack() {
    if (_editing) {
        _editing = false;
        _dirty = true;
    } else if (_stack->count > 0) {
        _current_screen_index = stack_pop(_stack);
        _cursor = 0;
        _dirty = true;

        if (_stack->count == 0) {
            _active = false; // return home
        }
    }
}

bool MenuNavigator::isDirty(){
    bool was_dirty = _dirty;
    _dirty = false;
    return was_dirty;
}

const menu_screen_t* MenuNavigator::getCurrentScreen() const {
    return &_screens[_current_screen_index];
}

uint8_t MenuNavigator::getCursor() const {
    return _cursor;
}

bool MenuNavigator::isEditingValue() const {
    return _editing;
}

void MenuNavigator::setActive(bool active) {
    if (active) {
        _dirty = true;
        _cursor = 0;
        _editing = false;
        _current_screen_index = 0;
        _edit_temp = 0;
        stack_clear(_stack);
        stack_push(_stack, _current_screen_index);
    }
    _active = active;

}