/**
 * \file pipower.cpp
 *
 * Raspberry Pi/Powerboost 1000c/ATtiny85  power controller
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "button.h"
#include "millis.h"
#include "input.h"
#include "pins.h"
#include "states.h"

/** \defgroup BitOps Bit operations
 * @{
 */

#define ROL8(N) ((uint8_t)(N << 1) | (N >> 7))      /**< Rotate left, 8-bit version */
#define ROR8(N) ((uint8_t)(N >> 1) | (N << 7))      /**< Rotate right, 8-bit version */
#define ROL16(N) ((uint16_t)(N << 1) | (N >> 15))   /**< Rotate left, 16-bit version */
#define ROR16(N) ((uint16_t)(N >> 1) | (N << 15))   /**< Rotate right, 16-bit version */

/** @} */

/** \defgroup Button Power button
 * @{
 */
#define LONG_PRESS_DURATION 2000    /**< Length of long press */
#define BUTTON_NORMAL 0
#define BUTTON_IGNORE 1             /**< Power button must be released */
/** @} */

/** \defgroup Timers Timers
 * @{
 */
#define ONE_SECOND 1000
#define TIMER_BUTTON 10                     /**< Period for reading button state */
#define TIMER_POWERWAIT (1 * ONE_SECOND)    /**< How long to wait for USB to stabilize */
#define TIMER_BOOTWAIT (30 * ONE_SECOND)    /**< How long to wait for boot */
#define TIMER_SHUTDOWN (30 * ONE_SECOND)    /**< How long to wait for shutdown */
#define TIMER_POWEROFF (30 * ONE_SECOND)    /**< How long to wait for power off */
#define TIMER_IDLE (5 * ONE_SECOND)

/** @} */

/** Set to the current value of `millis()` on each loop iteration. */
unsigned long now;

/** \addtogroup Timers
 * @{
 */
unsigned long timer_button = 0; /**< How often to check button state */
unsigned long timer_start = 0;  /**< Generic timer used in state transitions */
/** @} */

/** \addtogroup Button
 * @{
 */

Button power_button(PIN_POWER);
uint8_t power_button_state = BUTTON_NORMAL; /**< Power button current state */
unsigned long time_pressed;             /**< Current press duration */
/** @} */

/** Current run state.
  * \ingroup States
  */
uint8_t state = 0;

Input usb(PIN_USB, false);

EMPTY_INTERRUPT(PCINT0_vect);

/** Run once when mc boots. */
void setup() {
    // PIN_EN and PIN_SHUTDOWN are outputs
    DDRB = 1<<PIN_EN | 1<<PIN_SHUTDOWN;

    // Enable pullup on PIN_BOOT and PIN_POWER
    PORTB |= 1<<PIN_BOOT | 1<<PIN_POWER;

    // Enable pin change interrupts
    GIMSK |= 1<<PCIE;
    PCMSK |= 1<<(PIN_POWER) | 1<<(PIN_USB);

    init_millis();
}

/** Transition to a new state.
  *
  * This sets the global `state` variable.
  *
  * \ingroup States
  */
void to_state(uint8_t new_state) {
    state = new_state;
}

/** Runs periodically */
void loop() {
    uint8_t long_press = 0;
    uint8_t short_press = 0;

    now = millis();
    usb.update();

    if (now - timer_button > TIMER_BUTTON) {
        timer_button = now;

        // Manage power button
        power_button.update();

        if (power_button_state == BUTTON_IGNORE) {
            if (power_button.is_released()) {
                power_button_state = BUTTON_NORMAL;
            }
        } else if (power_button.is_pressed()) {
            time_pressed = now;
        } else if (power_button.is_released()) {
            short_press = 1;
        } else if (power_button.is_down()) {
            unsigned long delta = now - time_pressed;
            if (delta > LONG_PRESS_DURATION) {
                long_press = 1;
                power_button_state = BUTTON_IGNORE;
            }
        }
    }

    // At any point, a long press will force the power off.
    if (long_press) {
        to_state(STATE_POWEROFF2);
    }

    //PORTB = (PORTB & ~(1<<PIN_SHUTDOWN)) | (usb.is_high()?1:0)<<PIN_SHUTDOWN;

    switch(state) {
        case STATE_START:
            if (usb.is_high()) {
                // USB goes high briefly when the microcontroller starts
                // up. Wait a second for it to stabilize before we try to
                // boot.
                to_state(STATE_POWERWAIT0);
            } else {
                to_state(STATE_POWEROFF2);
            }
            break;

        case STATE_POWERWAIT0:
            // start powerwait timer
            timer_start = now;
            to_state(STATE_POWERWAIT1);
            break;

        case STATE_POWERWAIT1:
            if (usb.went_low()) {
                to_state(STATE_POWEROFF2);
            } else if (now - timer_start > TIMER_POWERWAIT) {
                to_state(STATE_POWERON);
            }
            break;

        case STATE_POWERON:
            // Assert EN
            PORTB |= 1<<PIN_EN;
            to_state(STATE_BOOTWAIT0);
            break;

        case STATE_BOOTWAIT0:
            // Start bootwait timer
            timer_start = now;
            to_state(STATE_BOOTWAIT1);
            break;

        case STATE_BOOTWAIT1:
            // Wait for Pi to assert BOOT or timeout
            if (now - timer_start > TIMER_BOOTWAIT) {
                to_state(STATE_POWEROFF2);
            } else if (!(PINB & (1<<PIN_BOOT))) {
                to_state(STATE_BOOT);
            }
            break;

        case STATE_BOOT:
            // Wait for power button or Pi to de-assert BOOT
            if (short_press || usb.went_low()) {
                to_state(STATE_SHUTDOWN0);
            } else if (PINB & (1<<PIN_BOOT)) {
                to_state(STATE_POWEROFF0);
            }
            break;

        case STATE_SHUTDOWN0:
            // Assert SHUTDOWN
            PORTB |= 1<<PIN_SHUTDOWN;
            timer_start = now;
            to_state(STATE_SHUTDOWN1);
            break;

        case STATE_SHUTDOWN1:
            // Wait for Pi to de-assert BOOT or timeout
            if (now - timer_start > TIMER_SHUTDOWN) {
                to_state(STATE_POWEROFF0);
            } else if (PINB & (1<<PIN_BOOT)) {
                to_state(STATE_POWEROFF0);
            }
            break;

        case STATE_POWEROFF0:
            // Start poweroff timer
            PORTB &= ~(1<<PIN_SHUTDOWN);
            timer_start = now;
            to_state(STATE_POWEROFF1);

        case STATE_POWEROFF1:
            // Wait for poweroff timer to expire.
            if (now - timer_start > TIMER_POWEROFF) {
                to_state(STATE_POWEROFF2);
            } else if (!(PINB & (1<<PIN_BOOT))) {
                // Pi has re-asserted BOOT
                to_state(STATE_BOOT);
            }
            break;

        case STATE_POWEROFF2:
            // De-assert EN and SHUTDOWN
            PORTB &= ~(1<<PIN_EN);
            PORTB &= ~(1<<PIN_SHUTDOWN);
            to_state(STATE_IDLE0);
            break;

        case STATE_IDLE0:
            // Enter low power mode.
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_mode();
            to_state(STATE_IDLE1);

        case STATE_IDLE1:
            timer_start = now;
            to_state(STATE_IDLE2);

        case STATE_IDLE2:
            if (now - timer_start > TIMER_IDLE) {
                // return to low power mode if nothing to do
                to_state(STATE_IDLE0);
            } else if (short_press && usb.is_high()) {
                // power on if power button pressed and power is available
                to_state(STATE_POWERON);
            } else if (usb.went_high()) {
                // power on if power is restored
                to_state(STATE_POWERON);
            }
            break;
    }
}

int main() {
	setup();

	while (1) {
		loop();
	}
}
