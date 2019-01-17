/**
 * \file button.c
 *
 * Simple debouncing routine from
 * <https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>

#include "button.h"

#define BUTTON_RELEASED 0b00000111  /**< History pattern when a button is released */
#define BUTTON_PRESSED  0b11000000  /**< History pattern when a button is pressed */
#define BUTTON_MASK     0b11000111  /**< Mask out noise in the button history */
#define BUTTON_UP       0b11111111  /**< History pattern when button is up */
#define BUTTON_DOWN     0b00000000  /**< History pattern when button is down */

typedef struct Button {
    uint8_t pin,        /**< Pin to which button is attached */
            history;    /**< Button state history */
} Button;

/** Create a new Button object. */
Button *button_new(uint8_t pin) {
    Button *button = (Button *)malloc(sizeof(Button));
    button->pin = pin;
    button->history = BUTTON_UP;

    // ensure pin is an input
    DDRB &= ~(1<<pin);

    // set pull-up
    PORTB |= 1<<pin;

    return button;
}

/** Push current button state onto history. */
void button_update(Button *button) {
    button->history = button->history << 1;
    button->history |= (PINB & (1<<button->pin))>>button->pin;
}

/** Return true if button has been pressed. */
bool button_is_pressed(Button *button) {
    bool pressed = 0;

    if ((button->history & BUTTON_MASK) == BUTTON_PRESSED) {
        pressed = 1;
        button->history = BUTTON_DOWN;
    }

    return pressed;
}

/** Return true if button has been released. */
bool button_is_released(Button *button) {
    bool released = 0;

    if ((button->history & BUTTON_MASK) == BUTTON_RELEASED) {
        released = 1;
        button->history = BUTTON_UP;
    }

    return released;
}

/** Return true if button is up */
bool button_is_up(Button *button) {
    return (button->history == BUTTON_UP);
}

/** Return true if button is down */
bool button_is_down(Button *button) {
    return (button->history == BUTTON_DOWN);
}
