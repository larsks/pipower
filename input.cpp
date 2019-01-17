/**
 * \file input.cpp
 */

#include <stdint.h>
#include <avr/io.h>

#include "input.h"

Input::Input(int _pin, bool pullup) {
    pin = _pin;

    // Ensure pin is an input.
    DDRB &= ~(1<<pin);

    // Set pullup if requested
    if (pullup) {
	    PORTB |= 1<<pin;
    }

    update();
    last_state = state;
}

void Input::update() {
    state = !(!(PINB & (1<<pin)));
}

bool Input::went_high() {
    bool ret = (!last_state && state);
    last_state = state;
    return ret;
}
bool Input::went_low() {
    bool ret = (last_state && !state);
    last_state = state;
    return ret;
}
bool Input::is_high() {
    return state;
}
bool Input::is_low() {
    return !state;
}
