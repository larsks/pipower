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

# simulate a short press of the power button
define short_press
    set PINB=PINB & ~(1<<PIN_POWER)
    wait_for 100
    set PINB=PINB | 1<<PIN_POWER
    c
end

# log a message
define log
	printf "\n* %s\n", $arg0
end

##
## Execution starts here
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
log "setting PIN_USB"
set PINB=PINB | 1<<PIN_USB
wait_for 100

# assert BOOT
log "resetting PIN_BOOT"
set PINB=PINB & ~(1<<PIN_BOOT)
disable 1
tb loop if STATE_BOOT == state
c
enable 1

##
## ...the pi has booted...
##
wait_for 1000

# request a shutdown by pressing the power button
log "pressing power button"
short_press
disable 1
tb loop if STATE_SHUTDOWN1 == state
c
enable 1

# de-assert BOOT
wait_for 100
log "setting PIN_BOOT"
set PINB=PINB | 1<<PIN_BOOT

# step through state transitions until we reach
# STATE_IDLE2
disable 1
tb loop if STATE_POWEROFF0 == state
c
tb loop if STATE_POWEROFF1 == state
c
tb loop if STATE_POWEROFF2 == state
c
tb loop if STATE_IDLE0 == state
c
log "entering idle mode"
tb loop if STATE_IDLE1 == state
c
tb loop if STATE_IDLE2 == state
c
enable 1

wait_for 100

log "setting quit flag"
set state=STATE_QUIT
finish

disconnect
quit
