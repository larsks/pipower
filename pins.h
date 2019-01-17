/**
 * \file pins.h
 *
 * Pin defintitions.
 */

#ifndef _pins_h
#define _pins_h

#define PIN_USB PB0         /**< [INPUT] USB signal from Powerboost */
#define PIN_POWER PB1       /**< [INPUT] Power button */
#define PIN_EN PB2          /**< [OUTPUT] EN to Powerboost */
#define PIN_BOOT PB3        /**< [INPUT] BOOT signal from Pi */
#define PIN_SHUTDOWN PB4    /**< [OUTPUT] SHUTDOWN signal to Pi */

#endif // _pins_h
