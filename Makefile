CC=gcc
CFLAGS=-Wall -Wextra -ansi -pedantic -pedantic-errors -O2

BINARIES=colorify

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

colorify:
	$(CC) $(CFLAGS) -o colorify colorify.c
