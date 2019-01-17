PROGNAME    = pipower
DEVICE      = attiny85
CLOCK       = 1000000
PROGRAMMER  = -c arduino 
OBJECTS     = button.o pipower.o millis.o input.o
FUSES       = -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 
PORT	    = -P /dev/ttyACM0 -b19200
AVRDUDE     = avrdude -v $(PORT) $(PROGRAMMER) -p $(DEVICE)

CC	= avr-gcc
CPP	= avr-g++
CFLAGS	= -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	$(PROGNAME).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

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
