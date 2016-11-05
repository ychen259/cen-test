#include "game.h"

static size_t rand_bound(size_t low, size_t high)
{
	size_t r;
	do {
		r = round(genrand64_real1());
	} while (r < low || r > high);
	return r;
}

/* Modern Fisher-Yates per Wikipedia.
 * /wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
*/
static void shuffle_tiles(struct tile *a, size_t top)
{
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	init_genrand64(tp.tv_nsec); /* Seed the PRNG for the shuffling. */

	for (size_t i = top - 1; i > 1; --i) {
		size_t j = rand_bound(0, i);
		struct tile swap = a[i];
		a[i] = a[j];
		a[j] = swap;
	}
}

static void init_deck(struct tile deck[TILE_COUNT])
{
	/* Tileset: http://russcon.org/RussCon/carcassonne/tiles.html */
	/* TODO: Refactor so that this fits in 80 columns. */
	size_t ind = 0;
	deck[ind++] = /* start tile, must be first. */
		make_tile((enum edge[5]){CITY, ROAD, FIELD, ROAD, ROAD},
		NONE);
	deck[ind++] =
		make_tile((enum edge[5]){CITY, CITY, CITY, CITY, CITY}, SHIELD);
	deck[ind++] =
		make_tile((enum edge[5]){ROAD, ROAD, ROAD, ROAD, ROAD}, NONE);
	for (int i = 0; i < 7; ++i) {
		const enum attribute a[7] =
			{NONE, NONE, NONE, SHIELD, NONE, SHIELD, SHIELD};
		const enum edge b[7] = {FIELD,FIELD,FIELD,FIELD,ROAD,ROAD,ROAD};
		deck[ind++] =
			make_tile((enum edge[5]){CITY, CITY, b[i], CITY, CITY},
			a[i]);
	}
	for (int i = 0; i < 4; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, ROAD, ROAD, ROAD, ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		const enum attribute a[3] = {NONE, SHIELD, SHIELD};
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, CITY, FIELD, CITY,CITY},
			a[i]);
	}
	for (int i = 0; i < 8; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){ROAD, FIELD, ROAD, FIELD,ROAD},
			NONE);
	}
	for (int i = 0; i < 10; ++i) {
		const enum attribute a[5] = {NONE, NONE, NONE, SHIELD, SHIELD};
		const enum edge b[2] = {ROAD, FIELD};
		deck[ind++] =
			make_tile((enum edge[5]){CITY,b[i%2],b[i%2],CITY,CITY},
			a[i]);
	}
	for (int i = 0; i < 9; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, FIELD, ROAD, ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 2; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,CITY,FIELD,FIELD,FIELD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,CITY,FIELD,CITY,FIELD},
			NONE);
	}
	for (int i = 0; i < 2; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,FIELD,ROAD,FIELD,FIELD},
			MONASTERY);
	}
	for (int i = 0; i < 4; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,FIELD,FIELD,FIELD,FIELD},
			MONASTERY);
	}
	for (int i = 0; i < 5; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,FIELD,FIELD,FIELD,FIELD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,ROAD,FIELD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,FIELD,ROAD,ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,ROAD,ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,FIELD,ROAD,ROAD},
			NONE);
	}
	assert(ind == TILE_COUNT);
	return;
}

struct game make_game(void)
{
	struct game g;
	init_deck(g.tile_deck);
	/* first index is required to be 0 (have to start with start tile). */
	shuffle_tiles(&g.tile_deck[1], TILE_COUNT - 1);
	g.graphs_used = 0;
	return g;
}

