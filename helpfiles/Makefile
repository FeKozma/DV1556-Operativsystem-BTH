CC=gcc
CFLAGS=-std=gnu99
LDFLAGS=-lpthread -lm
CFILES=$(wildcard *.c)
EXEFILES=$(patsubst %.c,%.out,$(CFILES))

all: $(EXEFILES)

%.out: %.c
	$(CC) $(CFLAGS) -o$@ $^ $(LDFLAGS)

clean:
	@rm -f $(EXEFILES)

