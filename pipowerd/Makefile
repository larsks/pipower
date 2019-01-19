OBJS = pipowerd.o
LIBS = -lwiringPi

%.pre: %.c
	$(CC) $(CFLAGS) -E -o $@ $<

all: pipowerd

pipowerd: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)
