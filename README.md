# PiPower

This contains code that will let an ATtiny85 act as a controller between a Raspberry Pi and an Adafruit [Powerboost 1000c][].

[powerboost 1000c]: https://www.adafruit.com/product/2465


## Systemd units

- `assert-boot.service`

  Asserts the `BOOT` signal when the Pi boots and de-asserts it on shutdown.

- `power-button.service`

  Responds to `SHUTDOWN` signal from power controller by powering off the Pi.

- `powerloss.service`

  Responds to loss of `USB` signal from power controller by starting the powerloss shutdown timer.  The timer will trigger a shutdown when it expires. If power is restored prior to the timer expiring, the timer will be cancelled.

- `powerloss-cancel.service`

  Cancels the powerloss shutdown timer.

- `powerloss-shutdown.service`

  Shuts down the system.  Triggered by the powerloss shutdown timer.

- `powerloss-shutdown.timer`

  This is the power loss shutdown timer. It is started by the `powerloss` service when a power loss is detected. Upon expiry it will trigger the `powerloss-shutdown` service.
