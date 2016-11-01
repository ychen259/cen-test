#include <stdio.h>
#include <string.h> /* memcpy */
#define AXIS 5 /* AXIS by AXIS board */

enum edge {
	NONE = 0,
	CITY = 1,
	FIELD = 2,
	ROAD = 3 
};

struct tile {
	enum edge edges[5]; /* Top, Right, Bottom, Left, Center. */
};

struct tile create_tile(enum edge edges[5])
{
	struct tile t;
	memcpy(&t.edges, edges, sizeof(edges[0]) * 5);
	return t;
}

struct tile rotate_tile(struct tile old, int rotation)
{
	enum edge new[5];
	new[4] = old.edges[4]; /* Center doesn't move. */
	
	for (unsigned int i = 4; i > 0; --i) { /* Need unsigned for wrapping */
		/* Note clockwise rotation moves previous elements forward. */
		new[i - 1] = old.edges[(i - 1 - rotation) % 4];
	}
	return create_tile(new);
}

void print_tile(struct tile t)
{
	/* Our array stores in clockwise order starting at the top.
	 * So the tile looks like this: (indexes)
	 * + 0 +
	 * 3 4 1
	 * + 2 +
	*/
	const char tiles[4] = {'X', 'C', 'F', 'R'};
	printf("+%c+\n", tiles[t.edges[0]]);
	printf("%c%c%c\n", tiles[t.edges[3]], 
			tiles[t.edges[4]], tiles[t.edges[1]]);
	printf("+%c+\n", tiles[t.edges[2]]);
}

struct slot {
	unsigned int x;
	unsigned int y;
};

struct slot make_slot(unsigned int x, unsigned int y)
{
	struct slot s = {
		.x = x,
		.y = y
	};
	return s;
}

int compare_slots(struct slot a, struct slot b)
{
	if (a.x < b.x) {
		return -1;
	} else if (a.x > b.x) {
		return 1;
	} else if (a.y < b.y) {
		return -1;
	} else if (a.y > b.y) {
		return 1;
	} else {
		return 0;
	}
}

struct board {
	struct tile tiles[AXIS*AXIS]; /* Arbitrary sizing */
	struct slot valid_slots[AXIS*AXIS]; /* Again arbitrary. */
	unsigned int valid_slot_count;
};

struct board make_board(void)
{
	struct board b;
	const enum edge edges[5] = { NONE, NONE, NONE, NONE, NONE };
	const unsigned int mid = (AXIS - 1) / 2; /* Gotta start in center. */
	b.valid_slots[0] = make_slot(mid, mid);
	b.valid_slot_count = 1;
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

int validate_slot(struct board b, struct slot s)
{
	unsigned int low = 0;
	unsigned int high = b.valid_slot_count;
	unsigned int m;
	while (low < high) {
		m = (low + high) / 2;
		switch(compare_slots(b.valid_slots[m], s)) {
		case -1:
			low = m + 1;
			break;
		case 1:
			high = m - 1;
			break;
		case 0:
			return m;
		}
	}
	return -1;
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

void play_move(struct board b, struct move m)
{
	b.tiles[m.slot.x * AXIS + m.slot.y] = rotate_tile(m.tile, m.rotation);
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
	print_board(b);

	return 0;
}
