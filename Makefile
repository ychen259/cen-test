all: board.c tile.o slot.o
	cc -O2 -o test board.c tile.o slot.o
tile.o: tile.c tile.h
	cc -O2 -c -o tile.o tile.c
slot.o: slot.c slot.h
	cc -O2 -c -o slot.o slot.c
