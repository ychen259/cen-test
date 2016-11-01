#include "tile.h"

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
	
	/* Clockwise rotation moves previous elements up, so we go backwards. */
	for (unsigned int i = 4; i > 0; --i) { /* Need unsigned for wrapping */
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
