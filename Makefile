CFLAGS=-std=c99 -march=native -flto -Wall -Wextra -pedantic -O2
all: game board server client

server: server.c game.o rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -o server server.c game.o rng.o tile.o board.o slot.o

client: client.c game.o rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -o client client.c game.o rng.o tile.o board.o slot.o

game: game.c rng.o tile.o board.o slot.o
	$(CC) $(CFLAGS) -DTEST -o test_game game.c rng.o tile.o board.o slot.o

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
