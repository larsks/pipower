/**
 * \file pins.h
 *
 * Pin defintitions.
 */

#ifndef _pins_h
#define _pins_h

#define PIN_POWER PB0       /**< [INPUT] Power button */
#define PIN_USB PB1         /**< [INPUT] USB signal from Powerboost */
#define PIN_EN PB2          /**< [OUTPUT] EN to Powerboost */
#define PIN_BOOT PB4        /**< [INPUT] BOOT signal from Pi */
#define PIN_SHUTDOWN PB3    /**< [OUTPUT] SHUTDOWN signal to Pi */

#endif // _pins_h
