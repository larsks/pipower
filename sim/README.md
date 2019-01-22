# Simulating pipower with simavr

1. Run `make` in the `sim` directory.

2. Start `simavr`:

        simavr -m attiny85  -f 1000000 pipower.elf  -t -g

3. Use `avr-gdb` to run `simulate.gdb`:

        avr-gdb -x simulate.gdb

This will generate `gtkwave_trace.vcd`, which you can view in `gtkwave` by running:

    gtkwave gtkwave_trace.vcd

You can view an existing trace by running `gtkwave` against the included save file:

    gtkwave pipower.gtkw
