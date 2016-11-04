CFLAGS=-std=c99 -march=native -flto -Wall -Wextra -pedantic -O2
all: board game

game: game.c mt19937-64.o tile.o
	$(CC) $(CFLAGS) -o test_game game.c mt19937-64.o tile.o

board: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -o test board.c tile.o slot.o move.o

mt19937-64.o: mt19937-64.c mt19937-64.h
	$(CC) $(CFLAGS) -c -o mt19937-64.o mt19937-64.c

move.o: move.c move.h
	$(CC) ${CFLAGS} -c -o move.o move.c

tile.o: tile.c tile.h
	$(CC) ${CFLAGS} -c -o tile.o tile.c

slot.o: slot.c slot.h
	$(CC) ${CFLAGS} -c -o slot.o slot.c
