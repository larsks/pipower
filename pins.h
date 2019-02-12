/**
 * \file pins.h
 *
 * Pin defintitions.
 */

#ifndef _pins_h
#define _pins_h

#include <avr/io.h>

#define PIN_POWER    _BV(PORTB0)
#define PIN_USB      _BV(PORTB1)
#define PIN_EN       _BV(PORTB2)
#define PIN_SHUTDOWN _BV(PORTB3)
#define PIN_BOOT     _BV(PORTB4)

#endif // _pins_h
