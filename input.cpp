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

/** Read the state of the pin */
void Input::update() {
    state = !(!(PINB & (1<<pin)));
}

/** Return true if the pin state has gone high.
 *
 * This checks if the pin state has changed since a previous call to
 * `went_high()` or `went_low()`.
 */
bool Input::went_high() {
    bool ret = (!last_state && state);
    last_state = state;
    return ret;
}

/** Return true if the pin state has gone low.
 *
 * This checks if the pin state has changed since a previous call to
 * `went_high()` or `went_low()`.
 */
bool Input::went_low() {
    bool ret = (last_state && !state);
    last_state = state;
    return ret;
}

/** Return true if the pin is high */
bool Input::is_high() {
    return state;
}

/** Return true if the pin is low */
bool Input::is_low() {
    return !state;
}
