OBJS = pipowerd.o

%.pre: %.c
	$(CC) $(CFLAGS) -E -o $@ $<

all: pipowerd

pipowerd: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f pipowerd $(OBJS)
