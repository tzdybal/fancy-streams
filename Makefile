CC=gcc
CFLAGS=-Wall -Wextra -ansi -pedantic -pedantic-errors -O2

BINARIES=colorify

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

colorify: colorify.c
	$(CC) $(CFLAGS) -o colorify colorify.c
