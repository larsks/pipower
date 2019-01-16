/**
 * \file button.h
 *
 * Simple debouncing routine from
 * <https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/>
 *
 */
#ifndef _button_h
#define _button_h

#include <stdint.h>

class Button {
    public:
        Button(int pin);
        void update();
        bool is_pressed();
        bool is_released();
        bool is_up();
        bool is_down();

    private:
        int pin;
        uint8_t history;
};

#endif
