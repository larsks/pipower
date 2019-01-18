PROGNAME    = pipower
DEVICE      = attiny85
CLOCK       = 1000000
PROGRAMMER  = -c arduino 
FUSE_LOW    = 0x62
FUSE_HIGH   = 0xdf
FUSE_EXT    = 0xff
FUSES       = -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXT):m 
AVR_PORT    = /dev/ttyACM0
AVR_BAUD    = 19200
PORT	    = -P $(AVR_PORT) -b $(AVR_BAUD)
AVRDUDE     = avrdude -v $(PORT) $(PROGRAMMER) -p $(DEVICE) $(AVR_EXTRA_ARGS)

OBJECTS = \
	pipower.o \
	button.o \
	input.o \
	millis.o

CC	= avr-gcc
CPP	= avr-g++
CFLAGS	= -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	$(PROGNAME).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

%.pre: %.c
	$(CPP) $(CFLAGS) -E $< -o $@

%.pre: %.cpp
	$(CPP) $(CFLAGS) -E $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:$(PROGNAME).hex:i

fuse:
	$(AVRDUDE) $(FUSES)

make: flash fuse

load: all
	bootloadHID $(PROGNAME).hex

clean:
	rm -f $(PROGNAME).hex $(PROGNAME).elf $(OBJECTS)

$(PROGNAME).elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGNAME).elf $(OBJECTS)

$(PROGNAME).hex: $(PROGNAME).elf
	rm -f $(PROGNAME).hex
	avr-objcopy -j .text -j .data -O ihex $(PROGNAME).elf $(PROGNAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(PROGNAME).elf
