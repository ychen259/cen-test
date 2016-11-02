#include <stdio.h>
#include <string.h>	/* memcpy */
#include "edge.h"	/* edges. */
#include "tile.h"	/* tiles. */
#include "slot.h"	/* slots. */

#define AXIS 5 /* AXIS by AXIS board */

struct board {
	struct tile tiles[AXIS*AXIS];
	struct slot slot_spots[AXIS*AXIS];
	unsigned int sps;
};

struct board make_board(void)
{
	struct board b;
	enum edge edges[5] = { NONE, NONE, NONE, NONE, NONE };
	const unsigned int mid = (AXIS - 1) / 2; /* Must start in center. */
	b.slot_spots[0] = make_slot(mid, mid);
	b.sps = 1;
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

static unsigned index_slot(struct slot s)
{
	return AXIS * s.x + s.y;
}

static int slot_placeable(struct board b, struct slot s)
{
	/* Binary search open positions for the desired one. */
	int low = 0;
	int high = b.sps;
	int m;
	while (low < high) {
		m = (low + high) / 2;
		switch(compare_slots(b.slot_spots[m], s)) {
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

static int slot_empty(struct board b, struct slot s)
{
	struct tile t = b.tiles[index_slot(s)];
	for (int i = 0; i < 5; ++i) {
		if (t.edges[i] != NONE) {
			return 0;
		}
	}
	return 1;
}

static int slot_on_board(struct slot s)
{
	if (s.x < AXIS && s.y < AXIS) {
		return 1;
	}
	return 0;
}

static struct board add_placeable_slot(struct board b, struct slot s)
{
	size_t i = 0;
	while (i < b.sps && compare_slots(s, b.slot_spots[i]) < 0) {
		i++;
	}
	if (i < b.sps) { /* Sorted insert. */
		memmove(&b.slot_spots[i + 1], &b.slot_spots[i],
				sizeof(s) * b.sps - i);
		b.slot_spots[i] = s;
	} else {
		b.slot_spots[i] = s;
	}
	b.sps++;
	return b;
}

static struct board remove_placeable_slot(struct board b, struct slot s)
{
	size_t i = 0;
	while (i < b.sps && compare_slots(s, b.slot_spots[i]) < 0) {
		i++;
	}
	memmove(&b.slot_spots[i], &b.slot_spots[i + 1], sizeof(s)*(b.sps - i));
	b.sps--;
	return b;
}

static void print_placeable_slots(struct board b)
{
	printf("Slots:\n");
	printf("X\tY\n");
	for (size_t i = 0; i < b.sps; ++i) {
		printf("%u\t%u\n", b.slot_spots[i].x, b.slot_spots[i].y);
	}
	return;
}

struct board update_slot_spots(struct board b, struct slot s)
{
	/* Check the slots above, left, right, and below. */
	struct slot adj[4] = { 
		make_slot(s.x, s.y - 1), make_slot(s.x - 1, s.y),
		make_slot(s.x + 1, s.y), make_slot(s.x, s.y + 1)
	};
	b = remove_placeable_slot(b, s);
	for (int i = 0; i < 4; ++i) {
		if (slot_empty(b, adj[i]) && slot_on_board(adj[i])) {
			b = add_placeable_slot(b, adj[i]);
		}
	}
	return b;
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
	if (!slot_placeable(b, m.slot)) {
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
	b.tiles[index_slot(m.slot)] = rotate_tile(m.tile, m.rotation);
	return update_slot_spots(b, m.slot);
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

	printf("\nTop row city. All invalid: \n");
	for (int i = 0; i < AXIS; ++i) {
		struct slot s = make_slot(i, 0);
		play_move(b, make_move(create_tile(city), s, 0));
	}

	printf("\nLet's see what slots are placeable.\n");
	print_placeable_slots(b);
	printf("\nPlay the center. Valid starting move.\n");
	b = play_move(b,
		make_move(create_tile(city), make_slot(AXIS/2,AXIS/2), 0));
	print_board(b);

	printf("\nAnd now what slots are placeable?\n");
	print_placeable_slots(b);

	return 0;
}
