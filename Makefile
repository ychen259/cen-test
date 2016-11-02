CFLAGS=-std=c99 -Wall -Wextra -pedantic -O2
all: board.c tile.o slot.o
	$(CC) $(CFLAGS) -o test board.c tile.o slot.o
tile.o: tile.c tile.h
	$(CC) ${CFLAGS} -c -o tile.o tile.c
slot.o: slot.c slot.h
	$(CC) ${CFLAGS} -c -o slot.o slot.c
