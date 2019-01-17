# PiPower

Turn an Adafruit [PowerBoost 1000c][] into a UPS with the help of an [ATtiny85][] microcontroller.

[powerboost 1000c]: https://www.adafruit.com/product/2465
[attiny85]: https://www.microchip.com/wwwproducts/en/ATtiny85

## Theory of operation

PiPower is designed to ensure that your Pi will shut down cleanly when it is disconnected from its primary power source.

PiPower runs on an ATtiny85 microcontroller and acts as an intermediary between your Raspberry Pi and an Adafruit PowerBoost 1000c.  When the microcontroller (mc) boots, it checks to see if an external power source is available to the PowerBoost.  If so, it sets the `EN` line and the PowerBoost starts to supply power to the Pi.

If no external power is available, the mc enters a low power sleep mode until external power is applied.  At this point, it will boot the Pi.

When the Pi boots, it must signal to the mc that it has booted successfully by bringing the `BOOT` line low. If the mc does not receive this signal within 30 seconds of providing power, it will turn off the power and return to the low power mode.

If external power is lost while the Pi is running, or if you press the power button while the Pi is running, the mc will send the `SHUTDOWN` signal to the Pi.  It will then wait up to 30 seconds for the Pi to shut down.  The Pi can signal a clean shutdown by setting the `BOOT` line high.  Once the shutdown is complete (or if 30 seconds pass), the mc will remove power from the Pi and return to low power mode.

## Pins

- `PB0` - `USB` line from PowerBoost
- `PB1` - power button
- `PB2` - `ENABLE` line to PowerBoost
- `PB3` - `BOOT` signal from Raspberry Pi
- `PB4` - `SHUTDOWN` signal to Raspberry Pi

## Systemd units

You can configure these services by creating the file `/etc/default/pipower` and setting one or more of `PIN_SHUTDOWN` and `PIN_BOOT`.

- `pipower-up.service`

  Asserts the `BOOT` signal on `PIN_BOOT` (default `GPIO4`) when the Pi boots and de-asserts it on shutdown.

- `pipower-down.service`

  Responds to the `SHUTDOWN` signal on `PIN_SHUTDOWN` (default `GPIO17`) from power controller by powering off the Pi.

## See also

- [PowerBoost 1000c data sheets/pinouts/etc](https://learn.adafruit.com/adafruit-powerboost-1000c-load-share-usb-charge-boost/downloads)
- [ATtiny85 data sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)

## License

PiPower, a UPS for your Raspberry Pi  
Copyright (C) 2019 Lars Kellogg-Stedman <lars@oddbit.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
