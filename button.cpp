/**
 * \file button.cpp
 *
 * Simple debouncing routine from
 * <https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/>
 */

#include <stdint.h>
#include <avr/io.h>

#include "button.h"

#define BUTTON_RELEASED 0b00000111
#define BUTTON_PRESSED  0b11000000
#define BUTTON_MASK     0b11000111
#define BUTTON_UP       0b11111111
#define BUTTON_DOWN     0b00000000

Button::Button(int _pin) {
    pin = _pin;
    history = BUTTON_UP;
    DDRB &= ~(1<<pin);
}

/** Push current button state onto history. */
void Button::update() {
    history = history << 1;
    history |= (PINB & (1<<pin))>>pin;
}

/** Return true if button has been pressed. */
bool Button::is_pressed() {
    bool pressed = 0;

    if ((history & BUTTON_MASK) == BUTTON_PRESSED) {
        pressed = 1;
        history = BUTTON_DOWN;
    }

    return pressed;
}

/** Return true if button has been released. */
bool Button::is_released() {
    bool released = 0;

    if ((history & BUTTON_MASK) == BUTTON_RELEASED) {
        released = 1;
        history = BUTTON_UP;
    }

    return released;
}

bool Button::is_up() {
    return (history == BUTTON_UP);
}

bool Button::is_down() {
    return (history == BUTTON_DOWN);
}
