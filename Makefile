CFLAGS=-std=c99 -g -march=native -flto -Wall -Wextra -pedantic -O0 -I rngs/
.PHONY: all clean

all: game board server client

clean:
	rm *.o

server: server.c game.o rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -pthread -o server server.c game.o rng.o tile.o board.o slot.o -lm

client: client.c game.o rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -o client client.c game.o rng.o tile.o board.o slot.o -lm

game: game.c rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -DTEST -o test_game game.c rng.o tile.o board.o slot.o -lm

board: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -DTEST -o test_board board.c tile.o slot.o move.o

board.o: board.c board.h tile.o slot.o move.o
	$(CC) $(CFLAGS) -c -o board.o board.c tile.o slot.o move.o

rng.o: rngs/mt19937-64.c rngs/mt19937-64.h
	$(CC) $(CFLAGS) -c -o rng.o rngs/mt19937-64.c

move.o: move.c move.h tile.o
	$(CC) ${CFLAGS} -c -o move.o move.c

tile.o: tile.c tile.h
	$(CC) ${CFLAGS} -c -o tile.o tile.c

slot.o: slot.c slot.h
	$(CC) ${CFLAGS} -c -o slot.o slot.c
