/**
 * \file pipower.cpp
 *
 * Raspberry Pi/Powerboost 1000c/ATtiny85  power controller
 */

#include <stdint.h>
#include <avr/io.h>
#include "button.h"
#include "millis.h"

/** \defgroup BitOps Bit operations
 * @{
 */

#define ROL8(N) ((uint8_t)(N << 1) | (N >> 7))      /**< Rotate left, 8-bit version */
#define ROR8(N) ((uint8_t)(N >> 1) | (N << 7))      /**< Rotate right, 8-bit version */
#define ROL16(N) ((uint16_t)(N << 1) | (N >> 15))   /**< Rotate left, 16-bit version */
#define ROR16(N) ((uint16_t)(N >> 1) | (N << 15))   /**< Rotate right, 16-bit version */

/** @} */

/** \defgroup Pins Pin definitions
 * @{
 */

#define PIN_LED PB0         /**< Status LED */
#define PIN_POWER PB1       /**< Power button */
#define PIN_EN PB2          /**< EN to Powerboost */
#define PIN_BOOT PB3        /**< BOOT signal from Pi */
#define PIN_SHUTDOWN PB4    /**< SHUTDOWN signal to Pi */

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
#define TIMER_BUTTON 10                    /**< Period for reading button state */
#define TIMER_LED 100                       /**< Period for LED update */
#define TIMER_BOOTWAIT (30 * ONE_SECOND)    /**< How long to wait for boot */
#define TIMER_SHUTDOWN (30 * ONE_SECOND)    /**< How long to wait for shutdown */
#define TIMER_POWEROFF (30 * ONE_SECOND)    /**< How long to wait for power off */

/** @} */

/** \defgroup States States
 * @{
 */

#define STATE_START     0   /**< Power has just been applied to mc */
#define STATE_POWERON   1   /**< Assert EN */
#define STATE_BOOTWAIT0 2   /**< Set bootwait timer */
#define STATE_BOOTWAIT1 3   /**< Wait for Pi to assert BOOT or timer expiry */
#define STATE_BOOT      4   /**< System has booted */
#define STATE_SHUTDOWN0 5   /**< Set shutdown timer */
#define STATE_SHUTDOWN1 6   /**< Wait for Pi to de-assert BOOT or timer expiry */
#define STATE_POWEROFF0 7   /**< Set poweroff timer */
#define STATE_POWEROFF1 8   /**< Wait for timer expiry */
#define STATE_POWEROFF2 9   /**< Power off */
#define STATE_IDLE      10  /**< Wait for power button press */

/** @} */

/** \defgroup LED LED Patterns
 * @{
 */

#define LED_ON          0b1111111111111111  /**< Always on */
#define LED_OFF         0b0000000000000000  /**< Always off */
#define LED_BLINK_SLOW  0b1111111100000000  /**< Blinking slowly */
#define LED_BLINK_FAST  0b1100110011001100  /**< Blinking rapidly */
#define LED_PULSE_LONG  0b1111000000000000  /**< Long pulses */
#define LED_PULSE_SHORT 0b1000000000000000  /**< Short pulses */

uint16_t led_pattern[STATE_IDLE + 1] = {0}; /**< LED pattern table */
uint16_t current_led_pattern = 0;           /**< Current LED pattern */

/** @} */

/** Set to the current value of `millis()` on each loop iteration. */
unsigned long now;

/** \addtogroup Timers
 * @{
 */
unsigned long timer_led = 0;    /**< How often to update LED pattern */
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

/** Run once when mc boots. */
void setup() {
    // PIN_LED, PIN_EN, and PIN_SHUTDOWN are outputs
    DDRB = 1<<PIN_LED | 1<<PIN_EN | 1<<PIN_SHUTDOWN;

    // Enable pullup on PIN_BOOT and PIN_POWER
    PORTB |= 1<<PIN_BOOT | 1<<PIN_POWER;

    led_pattern[STATE_BOOTWAIT0] = LED_BLINK_SLOW;
    led_pattern[STATE_BOOTWAIT1] = LED_BLINK_SLOW;
    led_pattern[STATE_BOOT] = LED_ON;
    led_pattern[STATE_SHUTDOWN0] = LED_BLINK_FAST;
    led_pattern[STATE_SHUTDOWN1] = LED_BLINK_FAST;
    led_pattern[STATE_POWEROFF0] = LED_PULSE_LONG;
    led_pattern[STATE_POWEROFF1] = LED_PULSE_LONG;
    led_pattern[STATE_POWEROFF2] = LED_PULSE_LONG;
    led_pattern[STATE_IDLE] = LED_PULSE_SHORT;

    init_millis();
}

/** Update LED state according to current pattern.
 *
 * \ingroup LED
 */
void update_led() {
    // Manage LED
    if (now - timer_led > TIMER_LED) {
        timer_led = now;
        if (current_led_pattern & 1) {
            PORTB |= 1<<PIN_LED;
        } else {
            PORTB &= ~(1<<PIN_LED);
        }
        current_led_pattern = ROR16(current_led_pattern);
    }
}

/** Transition to a new state.
  *
  * This sets the global `state` variable and updates `current_led_pattern`
  * from the `led_pattern` table.
  *
  * \ingroup States
  */
void to_state(uint8_t new_state) {
    state = new_state;
    current_led_pattern = led_pattern[state];
}

/** Runs periodically */
void loop() {
    uint8_t long_press = 0;
    uint8_t short_press = 0;

    now = millis();

    // Manage LED
    update_led();

    if (now - timer_button > TIMER_BUTTON) {
        timer_button = now;

        // Manage power button
        power_button.update();

        if (power_button_state == BUTTON_IGNORE) {
            if (power_button.is_pressed()) {
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

    if (long_press) {
        to_state(STATE_POWEROFF2);
    }

    switch(state) {
        case STATE_START:
            to_state(STATE_POWEROFF2);
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
            if (short_press) {
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
            to_state(STATE_IDLE);
            break;

        case STATE_IDLE:
            // Wait for power button.
            if (short_press) {
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
