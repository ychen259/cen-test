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

void print_tile(struct tile t)
{
	const char tiles[4] = {'X', 'C', 'F', 'R'};
	printf("+%c+\n", tiles[t.edges[0]]);
	printf("%c%c%c\n", tiles[t.edges[1]], 
			tiles[t.edges[3]], tiles[t.edges[4]]);
	printf("+%c+\n", tiles[t.edges[2]]);
}

struct board {
	struct tile tiles[16];
};

struct board make_board(void)
{
	struct board b;
	enum edge edges[5] = { NONE, NONE, NONE, NONE, NONE };
	for (int i = 0; i < 16; ++i) {
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
