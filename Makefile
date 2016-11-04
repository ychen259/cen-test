CFLAGS=-std=c99 -march=native -flto -Wall -Wextra -pedantic -O2
all: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -o test board.c tile.o slot.o move.o
move.o: move.c move.h
	$(CC) ${CFLAGS} -c -o move.o move.c
tile.o: tile.c tile.h
	$(CC) ${CFLAGS} -c -o tile.o tile.c
slot.o: slot.c slot.h
	$(CC) ${CFLAGS} -c -o slot.o slot.c
