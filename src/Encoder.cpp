#include <Encoder.h>

#define SS_SWITCH 24
#define SS_NEOPIX 6

#define SEESAW_ADDR 0x36

namespace Encoder
{
    /* The available encoder API is:
    *    int32_t getEncoderPosition();
         int32_t getEncoderDelta();
         void enableEncoderInterrupt();
         void disableEncoderInterrupt();
         void setEncoderPosition(int32_t pos);
    */

    static const uint32_t REFRESH_MS{100};
    static uint32_t last_execution_ms{millis()};
    static uint32_t button_wakeup_ms{millis()};
    static const uint32_t BUTTON_DEBOUNCE_MS{40};
    static const uint32_t BUTTON_LONG_MS{300};
    static volatile bool waiting_for_long{false};
    static volatile bool last_button_pressed{false};
    static volatile uint32_t button_pressed_time_ms{0};
    static volatile uint32_t button_released_time_ms{0};
    static volatile button_pressed_type_t button_press_type{NONE};
    static volatile bool light{false};

    Adafruit_seesaw ss;
    seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);

    int32_t encoder_delta{};

    void Setup()
    {
        log_i("Looking for seesaw!");

        if (!ss.begin(SEESAW_ADDR) || !sspixel.begin(SEESAW_ADDR))
        {
            log_e("Couldn't find seesaw on default address");
            while (1)
                delay(10);
        }
        log_i("seesaw started");

        sspixel.setBrightness(20);
        sspixel.show();

        // use a pin for the built in encoder switch
        ss.pinMode(SS_SWITCH, INPUT_PULLUP);

        ss.setEncoderPosition(0);
        // get starting position
        encoder_delta = -ss.getEncoderDelta();

        ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
        ss.enableEncoderInterrupt();
    }

    void Control() {
        uint32_t now{millis()};
        if (now - button_wakeup_ms > BUTTON_DEBOUNCE_MS){
            bool button_pressed = !ss.digitalRead(SS_SWITCH);
            button_wakeup_ms = now;
            if (button_pressed != last_button_pressed) {
                if (button_pressed) {
                    button_pressed_time_ms = now;
                    waiting_for_long = true;
                } else if (now - button_pressed_time_ms < BUTTON_LONG_MS) {
                    button_press_type = SHORT;
                    waiting_for_long = false;
                }
                last_button_pressed = button_pressed;
            }
            if (now - button_pressed_time_ms >= BUTTON_LONG_MS && waiting_for_long) {
                button_press_type = LONG;
                waiting_for_long = false;
            }
        }
        if (now - last_execution_ms >= REFRESH_MS) {
            encoder_delta = ss.getEncoderDelta();
            last_execution_ms = now;
        }
    }

    uint32_t Wheel(byte WheelPos)
    {
        WheelPos = 255 - WheelPos;
        if (WheelPos < 85)
        {
            return sspixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
        }
        if (WheelPos < 170)
        {
            WheelPos -= 85;
            return sspixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
        WheelPos -= 170;
        return sspixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }

    int32_t get_encoder_delta()
    {
        return -encoder_delta;
    }

    void set_encoder_position(int32_t new_encoder_position)
    {
        ss.setEncoderPosition(new_encoder_position);
    }

    button_pressed_type_t get_button_pressed()
    {   
        button_pressed_type_t press_type = button_press_type;
        reset_button();
        return press_type;
    }

    void reset_button()
    {
        button_press_type = NONE;
    }

    void toggle_light() {
        int r{0};
        int g{0};
        int b{0};
        light = !light;
        if (light) {
            r = 80;
            g = 80;
            b = 80;
        }
        sspixel.setPixelColor(0, sspixel.Color(r,g,b));
        sspixel.show();
    }
} // end of Encoder namespace