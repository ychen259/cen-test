#include <stdio.h>
#include <string.h>	/* memcpy */
#include "edge.h"	/* edges. */
#include "tile.h"	/* tiles. */
#include "slot.h"	/* slots. */

#define AXIS 5 /* AXIS by AXIS board */

struct board {
	struct tile tiles[AXIS*AXIS];
	struct slot placeable_slots[AXIS*AXIS];
	unsigned int placeable_slot_count;
};

struct board make_board(void)
{
	struct board b;
	const enum edge edges[5] = { NONE, NONE, NONE, NONE, NONE };
	const unsigned int mid = (AXIS - 1) / 2; /* Gotta start in center. */
	b.placeable_slots[0] = make_slot(mid, mid);
	b.placeable_slot_count = 1;
	for (unsigned int i = 0; i < AXIS*AXIS; ++i) {
		b.tiles[i] = create_tile(edges);
	}
	return b;
}

void print_board(struct board b)
{
	for (int i = 0; i < AXIS; ++i) {
		for (int j = 0; j < AXIS; ++j) {
			print_tile(b.tiles[i*AXIS + j]);
		}
		printf("\n");
	}
}

static int validate_slot(struct board b, struct slot s)
{
	/* Binary search open positions for the desired one. */
	int low = 0;
	int high = b.placeable_slot_count;
	int m;
	while (low < high) {
		m = (low + high) / 2;
		switch(compare_slots(b.placeable_slots[m], s)) {
		case -1:
			low = m + 1;
			break;
		case 1:
			high = m - 1;
			break;
		case 0:
			return m + 1; /* Guarenteed to be > 0. */
		}
	}
	return 0;
}

static void update_placeable_slots(struct board b, struct slot s)
{
	
}

struct move {
	struct tile tile;
	struct slot slot;
	int rotation;
};

struct move make_move(struct tile t, struct slot s, int rotation)
{
	struct move m = {
		.tile = t,
		.slot = s,
		.rotation = rotation
	};
	return m;
}

static int validate_move(struct board b, struct move m)
{
	if (!validate_slot(b, m.slot)) {
		return 1;
	}
	/* TODO: Add logic to ensure tile placements are correct */
	return 0;
}

struct board play_move(struct board b, struct move m)
{
	if (validate_move(b, m)) {
		printf("Invalid location for tile (%u, %u)!\n",
				m.slot.x, m.slot.y);
		return b;
	}
	printf("Valid location for tile (%u, %u)!\n", m.slot.x, m.slot.y);
	b.tiles[m.slot.x * AXIS + m.slot.y] = rotate_tile(m.tile, m.rotation);
	return b;
}

int main(void)
{
	printf("Testing different tile types.\n");
	printf("\nEmpty tile: \n");
	enum edge empty[5] = { NONE, NONE, NONE, NONE, NONE };
	print_tile(create_tile(empty));
	printf("\nAll Road tile: \n");
	enum edge road[5] = { ROAD, ROAD, ROAD, ROAD, ROAD };
	print_tile(create_tile(road));
	printf("\nAll Field tile: \n");
	enum edge field[5] = { FIELD, FIELD, FIELD, FIELD, FIELD };
	print_tile(create_tile(field));
	printf("\nAll City tile: \n");
	enum edge city[5] = { CITY, CITY, CITY, CITY, CITY };
	print_tile(create_tile(city));
	printf("\nMixed tile: \n");
	enum edge mixed[5] = { CITY, FIELD, ROAD, CITY, ROAD };
	print_tile(create_tile(mixed));
	for (int i = 1; i <= 4; ++i) {
		printf("\nRotate mixed tile %d: \n", i);
		print_tile(rotate_tile(create_tile(mixed), i));
	}

	printf("\nTesting board creation. All Null.\n");
	struct board b = make_board();
	print_board(b);

	printf("\nTop row city. \n");
	for (int i = 0; i < AXIS; ++i) {
		struct slot s = make_slot(i, 0);
		play_move(b, make_move(create_tile(city), s, 0));
	}
	b = play_move(b,
		make_move(create_tile(city), make_slot(AXIS/2,AXIS/2), 0));
	print_board(b);

	return 0;
}
