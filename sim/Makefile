CPPFLAGS += -I..
CFLAGS += -DTIMER_BOOTWAIT=1000 -DTIMER_SHUTDOWN=1000 -DTIMER_POWEROFF=1000
OFLAG ?= -Og
DEBUG ?= -g

VPATH = ..

ifeq ($(TRACE), 1)
OBJS = simavr.o
endif

%.svg: %.dot
	dot -T svg -o $@ $<

include ../Makefile
