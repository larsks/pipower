set pagination off
file pipower.elf
target remote :1234
load

##
## Helper functions
##

# wait for <n> milliseconds
define wait_for
    disable 1
    set $start_time = now
    tb loop if now == $start_time + $arg0
    c
    enable 1
end

# simulate a press of the power button
define press_power_button
    set PINB=PINB & ~(1<<PIN_POWER)
    wait_for 100
    set PINB=PINB | 1<<PIN_POWER
    c
end

##
## Things start here
##

# set an initial breakpoint at the start of loop() and advance the program
# to that point
b loop
c

# set up some information to display at each breakpoint
display state
display /t PORTB
display /t PINB
display

# let the code advance for 100ms
wait_for 100

# enable external power
set PINB=PINB | 1<<PIN_USB
wait_for 100

# assert BOOT
set PINB=PINB & ~(1<<PIN_BOOT)

##
## ...the pi has booted...
##
wait_for 1000

# request a shutdown by pressing the power button
press_power_button
wait_for 100

# de-assert BOOT
set PINB=PINB | 1<<PIN_BOOT
wait_for 100

# wait until we finish shutting down
disable 1
echo waiting to exit POWEROFF2
tb loop if state != STATE_POWEROFF1 && state != STATE_POWEROFF2
c
wait_for 100

echo setting quit flag
set state=STATE_QUIT
finish

disconnect
quit
