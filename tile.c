#include <stdio.h>
#include <string.h> /* memcpy */

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

struct board {
	struct tile tiles[4*4]; /* Arbitrary sizing */
};

struct board make_board(void)
{
	struct board b;
	enum edge edges[5] = { NONE, NONE, NONE, NONE, NONE };
	for (int i = 0; i < 4*4; ++i) {
		b.tiles[i] = create_tile(edges);
	}
	return b;
}

void print_board(struct board b)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			print_tile(b.tiles[i*4 + j]);
		}
		printf("\n");
	}
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
	for (int i = 0; i < 4; ++i) {
		b.tiles[i] = create_tile(city);
	}
	print_board(b);

	return 0;
}
