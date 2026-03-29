#include <Display.h>


DisplayRenderer::DisplayRenderer(U8G2& display, MenuNavigator& navigator) : _display(display), _navigator(navigator) {}

void DisplayRenderer::initialize() {
  log_i("Initializing display");   
  _display.setI2CAddress(0x7A);
  _display.begin();
  log_i("Display initialized");
}

void DisplayRenderer::renderLogo(){
  this->clear();
  _display.drawXBM( 0, 0, u8g_logo_width, u8g_logo_height, pot_bitmap);
  this->display();
}

void DisplayRenderer::renderHome(int current_temp, int target_temp, int target_duration, int runtime_m) {
    this->clear();
    _display.setDrawColor(1);
    _display.setFont(u8g2_font_t0_13b_tf);
    char target_temp_buffer[10] = {0};
    char current_temp_buffer[10] = {0};
    if (current_temp - target_temp > 2 || target_temp - current_temp > 2) {
      snprintf(current_temp_buffer, sizeof(target_temp_buffer), "%.1f°C", current_temp / 10.0);
      snprintf(target_temp_buffer, sizeof(target_temp_buffer), "%.1f°C", target_temp / 10.0);
    } else {
      snprintf(current_temp_buffer, sizeof(current_temp_buffer), "%.1f°C", current_temp / 10.0);
    }
    char time_buffer[10] = {0};
    int hours = (target_duration - runtime_m) / 60;
    int minutes = (target_duration - runtime_m) % 60;
    snprintf(time_buffer, sizeof(time_buffer), "%02dh%02dm", hours, minutes);

    _display.drawUTF8(0, 30, current_temp_buffer);
    _display.drawUTF8(0, 45, time_buffer);
    if (current_temp - target_temp > 2 || target_temp - current_temp > 2) {
      _display.drawUTF8(55, 30, target_temp_buffer);
      _display.setFont(u8g2_font_unifont_t_symbols);
      _display.drawGlyph(45, 30, 0x25b6);
    }
    this->display();
}

void DisplayRenderer::renderMenu(const menu_screen_t* screen) {
    this->clear();
    if (screen == NULL) {
        _display.drawUTF8(0, 15, "No screen");
        this->display();
        return;
    }
    drawTitle(screen);
    
    uint8_t cursor = _navigator.getCursor();
    bool editing = _navigator.isEditingValue();

    if (screen->item_count > 0 && cursor >= screen->item_count) {
        _navigator.setCursor(screen->item_count - 1);
    }
    
    if (screen->item_count == 0) {
        this->display();
        return;
    }
    
    if (cursor == screen->item_count-1) {
        // Show last 3 items
        uint8_t start_idx = (screen->item_count >= 3) ? screen->item_count - 3 : 0;
        for (uint8_t i = 0; i < 3 && start_idx + i < screen->item_count; i++) {
            drawItem(&(screen->items[start_idx + i]), i+1, (start_idx + i) == cursor, ((start_idx + i) == cursor) ? editing : false);
        }
    } else if (cursor == 0) {
        // Show first 3 items
        for (uint8_t i = 0; i < 3 && i < screen->item_count; i++) {
            drawItem(&(screen->items[i]), i+1, i == cursor, (i == cursor) ? editing : false);
        }
    } else {
        // Show cursor-1, cursor, cursor+1
        uint8_t start_idx = cursor - 1;
        for (uint8_t i = 0; i < 3 && start_idx + i < screen->item_count; i++) {
            drawItem(&(screen->items[start_idx + i]), i+1, (start_idx + i) == cursor, ((start_idx + i) == cursor) ? editing : false);
        }
    }
    this->display();
}

void DisplayRenderer::clear() {
    _display.clearBuffer();
}

void DisplayRenderer::display() {
    _display.sendBuffer();
}

void DisplayRenderer::drawTitle(const menu_screen_t* screen) {
  _display.setFont(u8g2_font_t0_13b_tf);
  const char* title = "No title";
  if (screen != NULL && screen->title != NULL) {
    title = screen->title;
  }
  _display.drawUTF8(0,FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX, title);
  _display.drawLine(0, FONT_HEIGHT_PX + 1, _display.getWidth(), FONT_HEIGHT_PX + 1);
}

void DisplayRenderer::drawItem(const menu_item_t* item, uint8_t line, bool selected, bool editing) {
  if (item == NULL) {
    return;
  }
  _display.setFont(u8g2_font_t0_13b_tf);
  const char* label = "No label";
  if (item->label != NULL) {
    label = item->label;
  }
  _display.drawUTF8(8, (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 2, label);
  uint8_t string_width = 0;
  char value_str[MAX_STRING_LENGTH + 1] = {0};
  int value = 0;
  u8g2_uint_t display_width = _display.getWidth();
  switch (item->type) {
    case MENU_ITEM_TYPE_VALUE:
      if (item->target.value.int_value) {
        value = *(item->target.value.int_value);
      }
      formatValue(value_str, item, editing);
      string_width = _display.getStrWidth(value_str);
      if (item->value_type_info && item->value_type_info->value_type == VALUE_TYPE_TEMP) {
        string_width -= 7; // Adjust for degree symbol if present
      // } else if (item->value_type_info && item->value_type_info->value_type == VALUE_TYPE_TEXT) {
      //   string_width = _display.getStrWidth("A")* (MAX_STRING_LENGTH - 1);
      }
      _display.drawUTF8(display_width - string_width, (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 2, value_str);
      break;
    case MENU_ITEM_TYPE_NAVIGATE:
      break;
    case MENU_ITEM_TYPE_ACTION:
       break;
    default:
      break;
  }
  if (selected) {
    _display.setFont(u8g2_font_unifont_t_symbols);
    _display.drawGlyph(0, (line + 1) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 1, 0x25b6);
    _display.setFont(u8g2_font_t0_13b_tf);
    if (item->type == MENU_ITEM_TYPE_VALUE) {
      if (editing) {
        int box_x, box_width;
        
        switch (item->value_type_info->value_type) {
            case VALUE_TYPE_TEMP:
                box_x = display_width - string_width;
                box_width = string_width;
                break;
            case VALUE_TYPE_TIME:
                if (_navigator.getEditPosition() == 0) {
                    box_width = this->getStrWidth(value_str, 2);
                    box_x = display_width - string_width;
                } else {
                    box_width = this->getStrWidth(value_str, 5) - this->getStrWidth(value_str, 3);
                    box_x = display_width - string_width + this->getStrWidth(value_str, 3);
                }
                break;
            case VALUE_TYPE_TEXT:
                char current_char[2] = {_navigator.getEditText()[_navigator.getEditPosition()], '\0'};
                box_width = _display.getStrWidth(current_char);
                box_x = display_width - string_width + this->getStrWidth(_navigator.getEditText(), _navigator.getEditPosition());
                break;
        }
        
        _display.setDrawColor(2);
        _display.drawBox(box_x, (line) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 1 , box_width, FONT_HEIGHT_PX + LINE_SPACING_PX - 1);
        _display.setDrawColor(1);
      }
    }
    _display.setDrawColor(2);
    _display.drawBox(0, (line) * FONT_HEIGHT_PX - FONT_GLYPH_LEG_PX + (line + 1) * LINE_SPACING_PX - 1 , display_width, FONT_HEIGHT_PX + LINE_SPACING_PX - 1);
    _display.setDrawColor(1);
  }
}


void DisplayRenderer::formatValue(char* buffer, const menu_item_t* item, bool editing) {
    size_t size = MAX_STRING_LENGTH + 1;
    if (!item || !item->value_type_info) {
        snprintf(buffer, size, "ERR");
        return;
    }
    int int_value = {0};
    char text_value[MAX_STRING_LENGTH + 1] = {0};
    switch (item->value_type_info->value_type) {
      case VALUE_TYPE_TEMP: {
        int_value = editing ? _navigator.getEditInt() : *(item->target.value.int_value);
        snprintf(buffer, size, "%.1f°C", int_value / 10.0);
        break;
      }
      case VALUE_TYPE_TIME: {
        int_value = editing ? _navigator.getEditInt() : *(item->target.value.int_value);
        int hours = int_value / 60;
        int minutes = int_value % 60;
        snprintf(buffer, size, "%02dh%02dm", hours, minutes);
        break;
      }
      case VALUE_TYPE_TEXT: {
        if (!item || !item->value_type_info) {
          snprintf(buffer, size, "ERR");
          return;
        }
        if (item->target.value.text_value) {
          strncpy(buffer, editing ? _navigator.getEditText() : (item->target.value.text_value ? item->target.value.text_value : ""), size - 1);
          if (editing && _navigator.getEditPosition() < MAX_STRING_LENGTH) {
              buffer[_navigator.getEditPosition()] = (char)_navigator.getEditInt();
          }
        }
        break;
      }
       default: {
        snprintf(buffer, size, "%d", int_value);
        break;
      }
    }
}

u8g2_uint_t DisplayRenderer::getStrWidth(const char* str, int uptochar) {
    if (str == NULL) {
        return 0;
    }
    if (uptochar < 0) {
        return _display.getStrWidth(str);
    } else {
        char buffer[MAX_STRING_LENGTH + 1] = {0};
        strncpy(buffer, str, uptochar);
        buffer[uptochar] = '\0';
        return _display.getStrWidth(buffer);
    }
}