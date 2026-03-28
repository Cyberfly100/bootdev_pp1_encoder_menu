#pragma once

#include <Arduino.h>
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

namespace Encoder
{
    typedef enum ButtonPressedType {
        NONE,
        SHORT,
        LONG
    } button_pressed_type_t;

    void Setup();

    void Control();

    uint32_t Wheel(byte WheelPos);

    int32_t get_encoder_delta();

    void set_encoder_position(int32_t new_encoder_position);

    button_pressed_type_t get_button_pressed();

    void reset_button();

    void toggle_light();

} // end of Encoder namespace