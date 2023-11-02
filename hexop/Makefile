CC=gcc
CFLAGS=-Wall -g

all: shift_hex

shift_hex: main.o
	$(CC) $(CFLAGS) main.o -o shift_hex

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o shift_hex
